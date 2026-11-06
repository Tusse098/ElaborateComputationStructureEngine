// systems/spawner.h
#pragma once
#include <ecs/fwd.h>
#include <ecs/core/world.h>

namespace game::systems {
    struct Spawner {
        int every = 5;
        int lifetime = 60;
        int tick = 0;
        void operator()(ecs::World& w, float dt);
    };
}