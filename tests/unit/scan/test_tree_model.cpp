#include <catch2/catch_all.hpp>

#include "scan/tree_model.hpp"
#include "util/string_intern.hpp"

using namespace vk::core;

TEST_CASE("tree builder aggregates sizes", "[scan][tree]") {
    TreeBuilder builder;
    auto& interner = StringInterner::pathInterner();

    FileEntry a;
    a.parentPath = interner.intern("root/dir");
    a.name = interner.intern("a.txt");
    a.logicalSize = 100;
    a.kind = FileKind::Regular;

    FileEntry b;
    b.parentPath = interner.intern("root/dir");
    b.name = interner.intern("b.txt");
    b.logicalSize = 200;
    b.kind = FileKind::Regular;

    builder.add(a);
    builder.add(b);

    auto tree = builder.build();
    REQUIRE(tree != nullptr);
    REQUIRE(tree->totalLogicalBytes == 300);
    REQUIRE(tree->totalFiles == 2);
}
