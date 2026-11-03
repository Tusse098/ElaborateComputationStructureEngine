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