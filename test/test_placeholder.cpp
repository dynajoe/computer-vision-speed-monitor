#include <catch2/catch.hpp>

TEST_CASE("placeholder case (valid)") {
    REQUIRE((1 + 1) == 2);
}

TEST_CASE("placeholder case (in-valid)") {
    REQUIRE((1 + 1) == 3);
}