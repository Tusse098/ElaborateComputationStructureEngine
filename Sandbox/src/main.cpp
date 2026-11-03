#include <ecs/core/world.h>
#include <ecs/core/pool.h>
#include <cstdio>
#include <core/components.h>



int main() {
    ecs::World w;
    ecs::Entity a = w.create();
    ecs::Entity b = w.create();

    w.defer().add(a, Position{1, 2});
    w.defer().add(a, Velocity{ 10, 0 });
    w.defer().add(b, Position{ 3, 4 });
    w.defer().add(a, Size{ 1, 1 });

    std::printf("\nShould not print a's components\n");
    w.each<Position, Velocity, Size>([](ecs::Entity e, Position& p, Velocity& v, Size& s) {
        std::printf("entity (id: %u, gen: %u) at (%g,%g)\n", ecs::index(e), ecs::generation(e), p.x, p.y);
        std::printf("Velocity (%g, %g)\n", v.x, v.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });
    w.flush();

    std::printf("\nShould print a's components\n");
    w.each<Position, Velocity, Size>([](ecs::Entity e, Position& p, Velocity& v, Size& s) {
        std::printf("entity (id: %u, gen: %u) at (%g,%g)\n", ecs::index(e), ecs::generation(e), p.x, p.y);
        std::printf("Velocity (%g, %g)\n", v.x, v.y);
        std::printf("Size (%g, %g)\n", s.x, s.y);
        });


}