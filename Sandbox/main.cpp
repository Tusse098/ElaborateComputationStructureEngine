#include <ecs/core/world.h>
#include <ecs/core/pool.h>s
#include <cstdio>
#include "sandbox.h"

int main() {
    ecs::World w;
    ecs::Entity a = w.create();
    ecs::Entity b = w.create();

    ecs::Pool<Position> positions;
    positions.add(a, Position{ 1, 2 });
    positions.add(b, Position{ 3, 4 });

    for (size_t i = 0; i < positions.size(); ++i)
        std::printf("entity %u at (%g, %g)\n", positions.owners()[i], positions.data()[i].x, positions.data()[i].y);
}