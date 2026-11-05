// Tests/entity_tests.cpp
#include <catch2/catch_test_macros.hpp>
#include <ecs/core/entity.h>

TEST_CASE("Entity: make/index/generation round-trip") {
    ecs::Entity e = ecs::make(7, 2);
    REQUIRE(ecs::index(e) == 7);
    REQUIRE(ecs::generation(e) == 2);
}

TEST_CASE("Entity: index and generation don't bleed into each other") {
    ecs::Entity e = ecs::make(ecs::INDEX_MASK, 255);   // both fields at max
    REQUIRE(ecs::index(e) == ecs::INDEX_MASK);
    REQUIRE(ecs::generation(e) == 255);
}

TEST_CASE("Entity: null is not the zero entity", "[entity]") {
    REQUIRE(ecs::null != ecs::Entity{});
    REQUIRE(ecs::index(ecs::null) == ecs::INDEX_MASK);
    REQUIRE(ecs::generation(ecs::null) == 0);
}

TEST_CASE("Entity: null survives a round-trip through make", "[entity]") {
    REQUIRE(ecs::make(ecs::index(ecs::null), ecs::generation(ecs::null)) == ecs::null);
}