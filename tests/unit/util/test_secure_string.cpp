#include <catch2/catch_all.hpp>

#include "util/secure_string.hpp"

using namespace vk::core;

TEST_CASE("secure string stores and clears", "[util][secure]") {
    SecureString secret { "topsecret" };
    REQUIRE(secret.view() == "topsecret");
    REQUIRE(secret.size() == 9);
    secret.clear();
    REQUIRE(secret.empty());
}
