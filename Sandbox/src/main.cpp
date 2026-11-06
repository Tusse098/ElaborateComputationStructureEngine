#include <ecs/core/world.h>
#include <ecs/core/pool.h>
#include <cstdio>
#include <core/components.h>
#include <ecs/debugging/debug.h>
#include <ecs/core/entity.h>
#include <ecs/core/scheduler.h>
#include <chrono>
#include "./core/systems/all.h"

using Clock = std::chrono::steady_clock;
auto seconds = [](auto d) { return std::chrono::duration<float>(d).count(); };

#define TOTALTICKS 1000
int main() {
    ecs::World w;
    ecs::Scheduler scheduler;
    game::addGameplaySystems(scheduler);

    for (int tick = 0; tick < 1000; ++tick) {
        scheduler.tick(w, 1.0f / 60);
        assert(ecs::debug::validate(w));
    }

    ecs::debug::printSummary(w);
    // expect: Position 12, Velocity 12, Lifetime 12, Frozen 4
}


