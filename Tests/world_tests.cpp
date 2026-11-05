// Tests/world_tests.cpp
#include <catch2/catch_test_macros.hpp>
#include <ecs/core/world.h>
#include <vector>

struct Position { float x, y; };
struct Size { float x, y; };
struct Frozen { };
struct Velocity { float x, y; };
struct Follow { ecs::Entity target = ecs::null; };


TEST_CASE("World: destroyed entity is not alive") {
    ecs::World w;
    auto e = w.create();
    w.destroy(e);
    REQUIRE_FALSE(w.alive(e));
}

TEST_CASE("World: destroying twice doesn't hand out the same slot twice") {
    ecs::World w;
    auto e = w.create();
    w.destroy(e);
    w.destroy(e);
    auto a = w.create();
    auto b = w.create();
    REQUIRE(a != b);
}

TEST_CASE("World: each hands out handles that are alive after slot reuse") {
    ecs::World w;
    auto old = w.create();
    w.destroy(old);
    auto fresh = w.create();
    w.add(fresh, Position{ 1, 2 });

    std::vector<ecs::Entity> seen;
    w.each<Position>([&](ecs::Entity e, Position&) { seen.push_back(e); });

    REQUIRE(seen.size() == 1);
    REQUIRE(seen[0] == fresh);
    REQUIRE(w.alive(seen[0]));
}

TEST_CASE("World: defer add waits and adds") {
    ecs::World w;
    auto a = w.create();
    auto b = w.create();

    w.defer().add(a, Position{ 1, 2 });
    w.defer().add(b, Position{ 1, 2 });
    REQUIRE(!w.has<Position>(a));
    REQUIRE(!w.has<Position>(b));
    w.flush();
    REQUIRE(w.has<Position>(a));
    REQUIRE(w.has<Position>(b));
}

TEST_CASE("World: defer add and remove") {
    ecs::World w;
    auto a = w.create();
    auto b = w.create();

    w.defer().add(a, Position{ 1, 2 });
    w.defer().add(b, Position{ 1, 2 });

    w.defer().remove<Position>(a);
    w.defer().destroy(b);

    REQUIRE(!w.has<Position>(a));
    REQUIRE(!w.has<Position>(b));

    w.flush();
    REQUIRE(!w.has<Position>(a));
    REQUIRE(!w.has<Position>(b));
}

TEST_CASE("World: commands recorded inside each apply after flush") {
    ecs::World w;
    for (int i = 0; i < 3; ++i) w.add(w.create(), Position{ float(i), 0 });

    int visited = 0;
    w.each<Position>([&](ecs::Entity e, Position&) {
        ++visited;
        w.defer().destroy(e);          // would corrupt the walk if immediate
        });
    REQUIRE(visited == 3);             // every entity seen exactly once

    int remaining = 0;
    w.each<Position>([&](ecs::Entity, Position&) { ++remaining; });
    REQUIRE(remaining == 3);           // nothing applied yet

    w.flush();
    remaining = 0;
    w.each<Position>([&](ecs::Entity, Position&) { ++remaining; });
    REQUIRE(remaining == 0);
}

TEST_CASE("World: null is never alive", "[world][null]") {
    ecs::World w;
    REQUIRE_FALSE(w.alive(ecs::null));          // empty world

    for (int i = 0; i < 100; ++i) w.create();
    REQUIRE_FALSE(w.alive(ecs::null));          // populated world
}

TEST_CASE("World: a handle this world never issued is not alive", "[world][null]") {
    ecs::World w;
    w.create();                                 // one slot exists
    REQUIRE_FALSE(w.alive(ecs::make(50, 0)));   // slot 50 doesn't
}

TEST_CASE("World: default-initialised reference means nobody", "[world][null]") {
    ecs::World w;
    auto a = w.create();
    w.add(a, Follow{});
    REQUIRE_FALSE(w.alive(w.get<Follow>(a).target));
}

TEST_CASE("World: destroying null is a no-op", "[world][null]") {
    ecs::World w;
    auto a = w.create();
    w.destroy(ecs::null);                       // must not assert or touch anything
    REQUIRE(w.alive(a));
}

TEST_CASE("World: exclude skips entities that have the excluded component", "[world][query]") {
    ecs::World w;

    auto moving = w.create(); w.add(moving, Position{}); w.add(moving, Velocity{});
    auto frozen = w.create(); w.add(frozen, Position{}); w.add(frozen, Velocity{}); w.add(frozen, Frozen{});

    std::vector<ecs::Entity> seen;
    w.each<Position, Velocity>(ecs::exclude<Frozen>, [&](ecs::Entity e, Position&, Velocity&) { seen.push_back(e); });

    REQUIRE(seen == std::vector{ moving });

    seen.clear();
    w.each<Position, Velocity>([&](ecs::Entity e, Position&, Velocity&) { seen.push_back(e); });

    std::sort(seen.begin(), seen.end());
    REQUIRE(seen == std::vector{ moving, frozen });
}
