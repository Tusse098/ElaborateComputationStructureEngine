// systems/movement.cpp
#include "movement.h"
#include <ecs/core/world.h>
#include <core/components.h>

void game::systems::movement(ecs::World& w, float dt) {
    w.each<Position, Velocity>(ecs::exclude<Frozen>, [&](ecs::Entity, Position& p, Velocity& v) {
        p.x += v.x * dt;  p.y += v.y * dt;
        });
}