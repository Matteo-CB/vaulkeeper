#include "journal/journal.hpp"

#include <sqlite3.h>

namespace vk::core {

namespace {

constexpr auto CreateTable = R"(
CREATE TABLE IF NOT EXISTS journal (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    kind INTEGER NOT NULL,
    ts_ns INTEGER NOT NULL,
    actor TEXT NOT NULL,
    subject TEXT NOT NULL,
    detail TEXT NOT NULL,
    size_bytes INTEGER NOT NULL DEFAULT 0,
    correlation TEXT NOT NULL DEFAULT ''
);
CREATE INDEX IF NOT EXISTS journal_ts ON journal(ts_ns);
CREATE INDEX IF NOT EXISTS journal_corr ON journal(correlation);
)";

Error sqlError(sqlite3* db, std::string_view context) {
    return makeError(ErrorCode::IoError, context, sqlite3_errmsg(db));
}

}

Journal::Journal() = default;

Journal::~Journal() {
    close();
}

Result<void> Journal::open(const std::filesystem::path& dbPath) {
    std::error_code ec;
    std::filesystem::create_directories(dbPath.parent_path(), ec);

    if (sqlite3_open(dbPath.string().c_str(), &db) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(db));
    }

    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA temp_store=MEMORY;", nullptr, nullptr, nullptr);

    char* err { nullptr };
    if (sqlite3_exec(db, CreateTable, nullptr, nullptr, &err) != SQLITE_OK) {
        Error e = makeError(ErrorCode::IoError, "journal schema init failed", err ? err : "");
        sqlite3_free(err);
        return std::unexpected<Error> { std::move(e) };
    }
    return ok();
}

void Journal::close() noexcept {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

Result<std::uint64_t> Journal::record(const JournalEntry& entry) {
    if (db == nullptr) {
        return fail(ErrorCode::InvalidArgument, "journal not open");
    }
    sqlite3_stmt* stmt { nullptr };
    constexpr auto sql = "INSERT INTO journal(kind, ts_ns, actor, subject, detail, size_bytes, correlation) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected<Error> { sqlError(db, "journal insert prepare") };
    }
    sqlite3_bind_int(stmt, 1, static_cast<int>(entry.kind));
    sqlite3_bind_int64(stmt, 2, toUnixNanos(entry.timestamp));
    sqlite3_bind_text(stmt, 3, entry.actor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, entry.subject.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, entry.detail.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, entry.sizeBytes);
    sqlite3_bind_text(stmt, 7, entry.correlationId.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        Error e = sqlError(db, "journal insert step");
        sqlite3_finalize(stmt);
        return std::unexpected<Error> { std::move(e) };
    }
    const auto rowId = static_cast<std::uint64_t>(sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);
    return rowId;
}

Result<std::vector<JournalEntry>> Journal::query(std::optional<TimePoint> since, std::optional<TimePoint> until, std::size_t limit) const {
    if (db == nullptr) {
        return fail(ErrorCode::InvalidArgument, "journal not open");
    }
    const auto sinceNs = since ? toUnixNanos(*since) : 0;
    const auto untilNs = until ? toUnixNanos(*until) : std::numeric_limits<std::int64_t>::max();

    sqlite3_stmt* stmt { nullptr };
    constexpr auto sql = "SELECT id, kind, ts_ns, actor, subject, detail, size_bytes, correlation "
                         "FROM journal WHERE ts_ns BETWEEN ? AND ? ORDER BY ts_ns DESC LIMIT ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected<Error> { sqlError(const_cast<sqlite3*>(db), "journal query prepare") };
    }
    sqlite3_bind_int64(stmt, 1, sinceNs);
    sqlite3_bind_int64(stmt, 2, untilNs);
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(limit));

    std::vector<JournalEntry> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        JournalEntry e;
        e.id = static_cast<std::uint64_t>(sqlite3_column_int64(stmt, 0));
        e.kind = static_cast<JournalEntryKind>(sqlite3_column_int(stmt, 1));
        e.timestamp = fromUnixNanos(sqlite3_column_int64(stmt, 2));
        e.actor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        e.subject = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        e.detail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        e.sizeBytes = sqlite3_column_int64(stmt, 6);
        e.correlationId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        out.push_back(std::move(e));
    }
    sqlite3_finalize(stmt);
    return out;
}

Result<std::vector<JournalEntry>> Journal::byCorrelation(std::string_view correlationId) const {
    if (db == nullptr) {
        return fail(ErrorCode::InvalidArgument, "journal not open");
    }
    sqlite3_stmt* stmt { nullptr };
    constexpr auto sql = "SELECT id, kind, ts_ns, actor, subject, detail, size_bytes, correlation "
                         "FROM journal WHERE correlation = ? ORDER BY ts_ns ASC;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected<Error> { sqlError(const_cast<sqlite3*>(db), "journal corr prepare") };
    }
    sqlite3_bind_text(stmt, 1, correlationId.data(), static_cast<int>(correlationId.size()), SQLITE_TRANSIENT);

    std::vector<JournalEntry> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        JournalEntry e;
        e.id = static_cast<std::uint64_t>(sqlite3_column_int64(stmt, 0));
        e.kind = static_cast<JournalEntryKind>(sqlite3_column_int(stmt, 1));
        e.timestamp = fromUnixNanos(sqlite3_column_int64(stmt, 2));
        e.actor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        e.subject = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        e.detail = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        e.sizeBytes = sqlite3_column_int64(stmt, 6);
        e.correlationId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        out.push_back(std::move(e));
    }
    sqlite3_finalize(stmt);
    return out;
}

Result<void> Journal::vacuum() {
    if (db == nullptr) {
        return fail(ErrorCode::InvalidArgument, "journal not open");
    }
    if (sqlite3_exec(db, "VACUUM;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return std::unexpected<Error> { sqlError(db, "journal vacuum") };
    }
    return ok();
}

}
