// systems/spawner.cpp
#include "spawner.h"
#include <core/components.h>
#include <iostream>

void game::systems::Spawner::operator()(ecs::World& w, float) {
    if (tick++ % every != 0) return;
    auto e = w.create();
    w.add(e, Position{}); w.add(e, Velocity{ 1, 0 }); w.add(e, Lifetime{ lifetime });
    if ((tick / every) % 3 == 0) w.add(e, Frozen{});
}