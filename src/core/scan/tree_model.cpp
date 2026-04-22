#include "scan/tree_model.hpp"

#include <algorithm>
#include <filesystem>

#include "util/string_intern.hpp"

namespace vk::core {

TreeBuilder::TreeBuilder() {
    rootNode = std::make_shared<ScanNode>();
    rootNode->name = StringInterner::pathInterner().intern("");
}

void TreeBuilder::add(const FileEntry& entry) {
    auto& interner = StringInterner::pathInterner();
    const auto fullPath = std::string(interner.view(entry.parentPath));
    std::filesystem::path parsed(fullPath);

    ScanNode* cursor = rootNode.get();
    for (const auto& segment : parsed) {
        const auto name = segment.generic_string();
        if (name.empty() || name == "/") { continue; }
        cursor = findOrInsertChild(*cursor, name);
    }

    if (entry.kind == FileKind::Directory) {
        (void)findOrInsertChild(*cursor, std::string(interner.view(entry.name)));
    } else {
        cursor->files.push_back(entry);
        cursor->totalFiles += 1;
        cursor->totalLogicalBytes += entry.logicalSize;
        cursor->totalSizeOnDisk += entry.sizeOnDisk == 0 ? entry.logicalSize : entry.sizeOnDisk;
    }
}

std::shared_ptr<ScanNode> TreeBuilder::build() {
    propagateTotals(*rootNode);
    return rootNode;
}

ScanNode* findOrInsertChild(ScanNode& parent, std::string_view name) {
    auto& interner = StringInterner::pathInterner();
    const auto handle = interner.intern(name);
    auto it = std::find_if(parent.children.begin(), parent.children.end(),
                           [&](const ScanNode& n) { return n.name == handle; });
    if (it != parent.children.end()) {
        return &*it;
    }
    ScanNode child;
    child.name = handle;
    parent.children.push_back(std::move(child));
    return &parent.children.back();
}

void propagateTotals(ScanNode& node) {
    std::uint64_t aggregatedFiles = node.totalFiles;
    std::uint64_t aggregatedLogical = node.totalLogicalBytes;
    std::uint64_t aggregatedDisk = node.totalSizeOnDisk;
    std::uint64_t aggregatedDirs = 0;

    for (auto& child : node.children) {
        propagateTotals(child);
        aggregatedFiles += child.totalFiles;
        aggregatedLogical += child.totalLogicalBytes;
        aggregatedDisk += child.totalSizeOnDisk;
        aggregatedDirs += 1 + child.totalDirectories;
    }

    node.totalFiles = aggregatedFiles;
    node.totalLogicalBytes = aggregatedLogical;
    node.totalSizeOnDisk = aggregatedDisk;
    node.totalDirectories = aggregatedDirs;
}

}
