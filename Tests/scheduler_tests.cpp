// scheduler_tests.cpp
#include <catch2/catch_test_macros.hpp>
#include <ecs/core/world.h>
#include <ecs/core/scheduler.h>
#include <vector>

namespace {
    struct Position { float x, y; };

    struct Counter {                       // stateful system: state must survive between ticks
        int calls = 0;
        void operator()(ecs::World&, float) { ++calls; }
    };
}

TEST_CASE("Scheduler: stateful system keeps its state across ticks", "[scheduler]") {
    ecs::World w;
    ecs::Scheduler s;
    s.add(Counter{});
    s.tick(w, 0.f);
    s.tick(w, 0.f);
    // can't reach the stored copy directly, so observe through the world instead:
    // a system that adds a component only on its second call
    struct SecondCall {
        int calls = 0;
        ecs::Entity e;
        void operator()(ecs::World& w, float) { if (++calls == 2) w.add(e, Position{}); }
    };
    auto e = w.create();
    ecs::Scheduler s2;
    s2.add(SecondCall{ .e = e });
    s2.tick(w, 0.f);
    REQUIRE_FALSE(w.has<Position>(e));
    s2.tick(w, 0.f);
    REQUIRE(w.has<Position>(e));         // fails with `for (auto s : systems)`
}

TEST_CASE("Scheduler: systems run in registration order", "[scheduler]") {
    ecs::World w;
    ecs::Scheduler s;
    std::vector<int> order;
    s.add([&](ecs::World&, float) { order.push_back(1); });
    s.add([&](ecs::World&, float) { order.push_back(2); });
    s.add([&](ecs::World&, float) { order.push_back(3); });
    s.tick(w, 0.f);
    REQUIRE(order == std::vector{ 1, 2, 3 });
}

TEST_CASE("Scheduler: tick flushes deferred commands", "[scheduler]") {
    ecs::World w;
    auto e = w.create();
    ecs::Scheduler s;
    s.add([&](ecs::World& w, float) { w.defer().add(e, Position{}); });
    s.tick(w, 0.f);
    REQUIRE(w.has<Position>(e));
}

TEST_CASE("Scheduler: deferred commands are not visible to later systems in the same tick", "[scheduler]") {
    ecs::World w;
    auto e = w.create();
    ecs::Scheduler s;
    bool seenByNext = true;
    s.add([&](ecs::World& w, float) { w.defer().add(e, Position{}); });
    s.add([&](ecs::World& w, float) { seenByNext = w.has<Position>(e); });
    s.tick(w, 0.f);
    REQUIRE_FALSE(seenByNext);          // flush happens after all systems, not between them
}

TEST_CASE("Scheduler: dt reaches every system unchanged", "[scheduler]") {
    ecs::World w;
    ecs::Scheduler s;
    float a = 0, b = 0;
    s.add([&](ecs::World&, float dt) { a = dt; });
    s.add([&](ecs::World&, float dt) { b = dt; });
    s.tick(w, 0.25f);
    REQUIRE(a == 0.25f);
    REQUIRE(b == 0.25f);
}

TEST_CASE("Scheduler: empty scheduler ticks without error", "[scheduler]") {
    ecs::World w;
    ecs::Scheduler s;
    s.tick(w, 0.f);
    REQUIRE(true);
}