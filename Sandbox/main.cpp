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
    w.add(a, Size{ 1, 1 });

    std::printf("\nShould print a's components\n");
    w.each<Position, Velocity, Size>([](ecs::Entity e, Position& p, Velocity& v, Size& s) {
        std::printf("entity %u at (%g,%g)\n", e, p.x, p.y);
        std::printf("Velocity (%g, %g)\n", v.x, v.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });

    w.remove<Velocity>(a);
    std::printf("\nShould print none\n");
    w.each<Position, Velocity, Size>([](ecs::Entity e, Position& p, Velocity& v, Size& s) {
        std::printf("entity %u at (%g,%g)\n", e, p.x, p.y);
        std::printf("Velocity (%g, %g)\n", v.x, v.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });

    std::printf("\nShould print a's components\n");
    w.each<Position, Size>([](ecs::Entity e, Position& p, Size& s) {
        std::printf("entity %u at (%g,%g)\n", e, p.x, p.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });

    w.destroy(a);

    std::printf("\nShould print none\n");
    w.each<Position, Size>([](ecs::Entity e, Position& p, Size& s) {
        std::printf("entity %u at (%g,%g)\n", e, p.x, p.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });

}