#include "scan/scan_cache.hpp"

#include <sqlite3.h>

namespace vk::core {

namespace {
constexpr auto CreateSchema = R"(
CREATE TABLE IF NOT EXISTS entries (
    record_number INTEGER PRIMARY KEY,
    usn_version INTEGER NOT NULL,
    logical_size INTEGER NOT NULL,
    modified_ns INTEGER NOT NULL,
    path_key TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS entries_path ON entries(path_key);
)";
}

ScanCache::ScanCache() = default;

ScanCache::~ScanCache() {
    close();
}

Result<void> ScanCache::open(const std::filesystem::path& dbPath) {
    std::error_code ec;
    std::filesystem::create_directories(dbPath.parent_path(), ec);
    if (sqlite3_open(dbPath.string().c_str(), &db) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(db));
    }
    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    if (sqlite3_exec(db, CreateSchema, nullptr, nullptr, nullptr) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(db));
    }
    return ok();
}

void ScanCache::close() noexcept {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

Result<void> ScanCache::upsert(const ScanCacheEntry& entry) {
    if (db == nullptr) { return fail(ErrorCode::InvalidArgument, "cache not open"); }
    sqlite3_stmt* stmt { nullptr };
    constexpr auto sql =
        "INSERT INTO entries(record_number, usn_version, logical_size, modified_ns, path_key) "
        "VALUES (?, ?, ?, ?, ?) "
        "ON CONFLICT(record_number) DO UPDATE SET "
        "usn_version = excluded.usn_version, "
        "logical_size = excluded.logical_size, "
        "modified_ns = excluded.modified_ns, "
        "path_key = excluded.path_key;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(db));
    }
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(entry.recordNumber));
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(entry.usnVersion));
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(entry.logicalSize));
    sqlite3_bind_int64(stmt, 4, entry.modifiedNs);
    sqlite3_bind_text(stmt, 5, entry.pathKey.c_str(), -1, SQLITE_TRANSIENT);
    const auto rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        return fail(ErrorCode::IoError, "cache upsert failed");
    }
    return ok();
}

Result<std::optional<ScanCacheEntry>> ScanCache::findByRecord(std::uint64_t recordNumber) const {
    if (db == nullptr) { return fail(ErrorCode::InvalidArgument, "cache not open"); }
    sqlite3_stmt* stmt { nullptr };
    constexpr auto sql = "SELECT usn_version, logical_size, modified_ns, path_key FROM entries WHERE record_number = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(const_cast<sqlite3*>(db)));
    }
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(recordNumber));
    std::optional<ScanCacheEntry> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ScanCacheEntry e;
        e.recordNumber = recordNumber;
        e.usnVersion = static_cast<std::uint32_t>(sqlite3_column_int64(stmt, 0));
        e.logicalSize = static_cast<std::uint64_t>(sqlite3_column_int64(stmt, 1));
        e.modifiedNs = sqlite3_column_int64(stmt, 2);
        e.pathKey = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result = std::move(e);
    }
    sqlite3_finalize(stmt);
    return result;
}

Result<void> ScanCache::vacuum() {
    if (db == nullptr) { return fail(ErrorCode::InvalidArgument, "cache not open"); }
    if (sqlite3_exec(db, "VACUUM;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        return fail(ErrorCode::IoError, sqlite3_errmsg(db));
    }
    return ok();
}

}
