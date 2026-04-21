#include "fs/reparse.hpp"

namespace vk::core {

Result<void> createJunction(const std::filesystem::path& linkPath, const std::filesystem::path& target) {
    std::error_code ec;
    std::filesystem::create_directory_symlink(target, linkPath, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    return ok();
}

Result<void> createSymbolicLink(const std::filesystem::path& linkPath, const std::filesystem::path& target, bool isDirectory) {
    std::error_code ec;
    if (isDirectory) {
        std::filesystem::create_directory_symlink(target, linkPath, ec);
    } else {
        std::filesystem::create_symlink(target, linkPath, ec);
    }
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    return ok();
}

Result<std::filesystem::path> readReparsePoint(const std::filesystem::path& path) {
    std::error_code ec;
    auto target = std::filesystem::read_symlink(path, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    return target;
}

Result<void> removeReparsePoint(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    return ok();
}

}
