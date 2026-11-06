// systems/all.cpp
#include "all.h"
#include "spawner.h"
#include "movement.h"
//#include "lifetime.h"
#include <ecs/core/scheduler.h>

void game::addGameplaySystems(ecs::Scheduler& s) {
    s.add(systems::Spawner{ .every = 5 });
    s.add(systems::movement);
    //s.add(systems::lifetime);
}