#include <ecs/core/world.h>
#include <ecs/core/pool.h>s
#include <cstdio>
#include "sandbox.h"



int main() {
    ecs::World w;
    ecs::Entity a = w.create();
    ecs::Entity b = w.create();

    w.add(a, Position{ 1, 2 });
    w.add(a, Velocity{ 10, 0 });
    w.add(b, Position{ 3, 4 });

    std::printf("a pos (%g,%g)  a has vel: %d b pos (%g, %g) b has vel: %d\n",
        w.get<Position>(a).x, w.get<Position>(a).y, w.has<Velocity>(a),w.get<Position>(b).x, w.get<Position>(b).y, w.has<Velocity>(b));

    w.get<Position>(b).x = 99;                 // get returns a reference, so this writes through
    std::printf("b pos.x after edit: %g\n", w.get<Position>(b).x);

    w.destroy(a);
    std::printf("after destroy: a has pos: %d  a has vel: %d  b has pos: %d\n",
        w.has<Position>(a), w.has<Velocity>(a), w.has<Position>(b));
}