#include "quarantine/quarantine.hpp"

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>

#include <fmt/format.h>

namespace vk::core {

namespace {

std::string makeItemId() {
    static thread_local std::mt19937_64 rng { std::random_device {}() };
    const auto hi = rng();
    const auto lo = rng();
    return fmt::format("{:016x}{:016x}", hi, lo);
}

std::string escapeJson(std::string_view value) {
    std::string out;
    out.reserve(value.size());
    for (const char c : value) {
        switch (c) {
            case '"': out.append("\\\""); break;
            case '\\': out.append("\\\\"); break;
            case '\b': out.append("\\b"); break;
            case '\f': out.append("\\f"); break;
            case '\n': out.append("\\n"); break;
            case '\r': out.append("\\r"); break;
            case '\t': out.append("\\t"); break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    out.append(fmt::format("\\u{:04x}", static_cast<unsigned>(c)));
                } else {
                    out.push_back(c);
                }
        }
    }
    return out;
}

struct JsonScanner {
    std::string_view input;
    std::size_t cursor { 0 };

    void skipWhitespace() {
        while (cursor < input.size()) {
            const char c = input[cursor];
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t') { ++cursor; continue; }
            break;
        }
    }

    [[nodiscard]] bool consume(char c) {
        skipWhitespace();
        if (cursor < input.size() && input[cursor] == c) { ++cursor; return true; }
        return false;
    }

    [[nodiscard]] bool peek(char c) {
        skipWhitespace();
        return cursor < input.size() && input[cursor] == c;
    }

    [[nodiscard]] std::optional<std::string> readString() {
        skipWhitespace();
        if (cursor >= input.size() || input[cursor] != '"') { return std::nullopt; }
        ++cursor;
        std::string out;
        while (cursor < input.size()) {
            const char c = input[cursor++];
            if (c == '"') { return out; }
            if (c == '\\' && cursor < input.size()) {
                const char next = input[cursor++];
                switch (next) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/': out.push_back('/'); break;
                    case 'b': out.push_back('\b'); break;
                    case 'f': out.push_back('\f'); break;
                    case 'n': out.push_back('\n'); break;
                    case 'r': out.push_back('\r'); break;
                    case 't': out.push_back('\t'); break;
                    default: out.push_back(next); break;
                }
            } else {
                out.push_back(c);
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<std::int64_t> readInt() {
        skipWhitespace();
        const auto start = cursor;
        if (cursor < input.size() && (input[cursor] == '-' || input[cursor] == '+')) { ++cursor; }
        while (cursor < input.size() && input[cursor] >= '0' && input[cursor] <= '9') { ++cursor; }
        if (cursor == start) { return std::nullopt; }
        try {
            return std::stoll(std::string(input.substr(start, cursor - start)));
        } catch (...) {
            return std::nullopt;
        }
    }

    [[nodiscard]] std::optional<bool> readBool() {
        skipWhitespace();
        if (input.substr(cursor, 4) == "true") { cursor += 4; return true; }
        if (input.substr(cursor, 5) == "false") { cursor += 5; return false; }
        return std::nullopt;
    }

    [[nodiscard]] bool readKey(std::string_view expected) {
        skipWhitespace();
        auto key = readString();
        if (!key || *key != expected) { return false; }
        return consume(':');
    }
};

}

Result<void> Quarantine::initialize(std::filesystem::path storageRoot, std::chrono::hours retentionDuration) {
    root = std::move(storageRoot);
    retention = retentionDuration;
    std::error_code ec;
    std::filesystem::create_directories(root, ec);
    if (ec) {
        return fail(ErrorCode::IoError, ec.message());
    }
    return loadIndex();
}

Result<QuarantineItem> Quarantine::capture(const std::filesystem::path& source, std::string reason, std::string correlationId) {
    std::error_code ec;
    if (!std::filesystem::exists(source, ec)) {
        return fail(ErrorCode::NotFound, "source does not exist");
    }

    const auto absoluteSource = std::filesystem::absolute(source, ec);
    const auto isDir = std::filesystem::is_directory(absoluteSource, ec);
    const auto id = makeItemId();
    const auto target = root / id;

    std::uint64_t originalSize = 0;
    std::filesystem::create_directories(target, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }

    if (isDir) {
        for (auto it = std::filesystem::recursive_directory_iterator(absoluteSource, ec);
             it != std::filesystem::recursive_directory_iterator(); ++it) {
            if (ec) { break; }
            if (it->is_regular_file(ec)) {
                originalSize += it->file_size(ec);
            }
        }
    } else {
        originalSize = std::filesystem::file_size(absoluteSource, ec);
    }

    std::filesystem::rename(absoluteSource, target / absoluteSource.filename(), ec);
    if (ec) {
        std::filesystem::remove_all(target);
        return fail(ErrorCode::IoError, ec.message());
    }

    QuarantineItem item {
        .id = id,
        .originalPath = absoluteSource,
        .storagePath = target,
        .originalSizeBytes = originalSize,
        .storedSizeBytes = originalSize,
        .capturedAt = SystemClock::now(),
        .expiresAt = SystemClock::now() + retention,
        .compressed = false,
        .reason = std::move(reason),
        .correlationId = std::move(correlationId)
    };

    items.push_back(item);
    storedBytes += item.storedSizeBytes;
    auto persisted = persistIndex();
    if (!persisted) {
        return fail(persisted.error());
    }
    return item;
}

Result<void> Quarantine::restore(std::string_view itemId) {
    auto it = std::find_if(items.begin(), items.end(), [&](const QuarantineItem& q) { return q.id == itemId; });
    if (it == items.end()) {
        return fail(ErrorCode::NotFound, "quarantine item not found");
    }

    std::error_code ec;
    std::filesystem::create_directories(it->originalPath.parent_path(), ec);
    const auto storedChild = it->storagePath / it->originalPath.filename();
    std::filesystem::rename(storedChild, it->originalPath, ec);
    if (ec) {
        return fail(ErrorCode::IoError, ec.message());
    }
    std::filesystem::remove_all(it->storagePath, ec);

    storedBytes -= std::min(storedBytes, it->storedSizeBytes);
    items.erase(it);
    return persistIndex();
}

Result<void> Quarantine::discard(std::string_view itemId) {
    auto it = std::find_if(items.begin(), items.end(), [&](const QuarantineItem& q) { return q.id == itemId; });
    if (it == items.end()) {
        return fail(ErrorCode::NotFound, "quarantine item not found");
    }
    std::error_code ec;
    std::filesystem::remove_all(it->storagePath, ec);
    storedBytes -= std::min(storedBytes, it->storedSizeBytes);
    items.erase(it);
    return persistIndex();
}

Result<std::size_t> Quarantine::purgeExpired(TimePoint now) {
    std::size_t removed = 0;
    for (auto it = items.begin(); it != items.end();) {
        if (now >= it->expiresAt) {
            std::error_code ec;
            std::filesystem::remove_all(it->storagePath, ec);
            storedBytes -= std::min(storedBytes, it->storedSizeBytes);
            it = items.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    auto persisted = persistIndex();
    if (!persisted) {
        return fail(persisted.error());
    }
    return removed;
}

Result<std::vector<QuarantineItem>> Quarantine::list() const {
    return items;
}

std::uint64_t Quarantine::totalStoredBytes() const {
    return storedBytes;
}

std::filesystem::path Quarantine::indexFile() const {
    return root / "index.json";
}

Result<void> Quarantine::persistIndex() const {
    std::ofstream os(indexFile(), std::ios::trunc);
    if (!os) {
        return fail(ErrorCode::IoError, "quarantine index write failed");
    }
    os << "{\n  \"items\": [\n";
    for (std::size_t i = 0; i < items.size(); ++i) {
        const auto& e = items[i];
        os << "    {"
           << "\"id\":\"" << escapeJson(e.id) << "\","
           << "\"original\":\"" << escapeJson(e.originalPath.generic_string()) << "\","
           << "\"storage\":\"" << escapeJson(e.storagePath.generic_string()) << "\","
           << "\"original_size\":" << e.originalSizeBytes << ","
           << "\"stored_size\":" << e.storedSizeBytes << ","
           << "\"captured_ns\":" << toUnixNanos(e.capturedAt) << ","
           << "\"expires_ns\":" << toUnixNanos(e.expiresAt) << ","
           << "\"compressed\":" << (e.compressed ? "true" : "false") << ","
           << "\"reason\":\"" << escapeJson(e.reason) << "\","
           << "\"correlation\":\"" << escapeJson(e.correlationId) << "\""
           << "}"
           << (i + 1 == items.size() ? "" : ",") << "\n";
    }
    os << "  ]\n}\n";
    return ok();
}

Result<void> Quarantine::loadIndex() {
    items.clear();
    storedBytes = 0;
    const auto path = indexFile();
    if (!std::filesystem::exists(path)) {
        return ok();
    }

    std::ifstream is(path);
    if (!is) {
        return fail(ErrorCode::IoError, "quarantine index read failed");
    }
    std::stringstream buffer;
    buffer << is.rdbuf();
    const auto raw = buffer.str();

    JsonScanner scanner { raw, 0 };
    if (!scanner.consume('{')) { return ok(); }
    if (!scanner.readKey("items")) { return ok(); }
    if (!scanner.consume('[')) { return ok(); }

    while (!scanner.peek(']')) {
        if (!scanner.consume('{')) { break; }
        QuarantineItem item;
        bool valid = true;
        while (valid && !scanner.peek('}')) {
            scanner.skipWhitespace();
            auto key = scanner.readString();
            if (!key || !scanner.consume(':')) { valid = false; break; }
            if (*key == "id") {
                auto v = scanner.readString(); if (!v) { valid = false; break; } item.id = *v;
            } else if (*key == "original") {
                auto v = scanner.readString(); if (!v) { valid = false; break; } item.originalPath = std::filesystem::path(*v);
            } else if (*key == "storage") {
                auto v = scanner.readString(); if (!v) { valid = false; break; } item.storagePath = std::filesystem::path(*v);
            } else if (*key == "original_size") {
                auto v = scanner.readInt(); if (!v) { valid = false; break; } item.originalSizeBytes = static_cast<std::uint64_t>(*v);
            } else if (*key == "stored_size") {
                auto v = scanner.readInt(); if (!v) { valid = false; break; } item.storedSizeBytes = static_cast<std::uint64_t>(*v);
            } else if (*key == "captured_ns") {
                auto v = scanner.readInt(); if (!v) { valid = false; break; } item.capturedAt = fromUnixNanos(*v);
            } else if (*key == "expires_ns") {
                auto v = scanner.readInt(); if (!v) { valid = false; break; } item.expiresAt = fromUnixNanos(*v);
            } else if (*key == "compressed") {
                auto v = scanner.readBool(); if (!v) { valid = false; break; } item.compressed = *v;
            } else if (*key == "reason") {
                auto v = scanner.readString(); if (!v) { valid = false; break; } item.reason = *v;
            } else if (*key == "correlation") {
                auto v = scanner.readString(); if (!v) { valid = false; break; } item.correlationId = *v;
            } else {
                valid = false;
                break;
            }
            if (!scanner.consume(',') && !scanner.peek('}')) { valid = false; break; }
        }
        if (!scanner.consume('}')) { break; }
        if (valid) {
            storedBytes += item.storedSizeBytes;
            items.push_back(std::move(item));
        }
        if (!scanner.consume(',') && !scanner.peek(']')) { break; }
    }

    return ok();
}

}
