#pragma once

#include <memory>
#include <string_view>

#include "fs/file_entry.hpp"

namespace vk::core {

class TreeBuilder {
public:
    TreeBuilder();

    VK_NONCOPYABLE(TreeBuilder);
    VK_NONMOVABLE(TreeBuilder);

    void add(const FileEntry& entry);
    [[nodiscard]] std::shared_ptr<ScanNode> build();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

[[nodiscard]] ScanNode* findOrInsertChild(ScanNode& parent, std::string_view name);
void propagateTotals(ScanNode& node);

}
