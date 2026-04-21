#include <catch2/catch_all.hpp>

#include "util/string_intern.hpp"

using namespace vk::core;

TEST_CASE("interner deduplicates", "[util][intern]") {
    StringInterner interner;
    const auto a = interner.intern("hello");
    const auto b = interner.intern("hello");
    const auto c = interner.intern("world");

    REQUIRE(a == b);
    REQUIRE(a != c);
    REQUIRE(interner.view(a) == "hello");
    REQUIRE(interner.view(c) == "world");
}
