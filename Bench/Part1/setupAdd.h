// bench/main.cpp
#include <ecs/core/world.h>
#include <cstdio>
#include <timer.h>
#include <components.h>
#include <include/oop.h>
#include <algorithm>
#include <random>

int main() {
    const int N = 100'00000;
    const int passes = 100;
    const float dt = 0.016f;
    ecs::World w;
    std::vector<GameObject*> objs;

    std::printf("\n------------------ECS----------------------\n");
    {
        Timer t;
        // create N entities, add Position{i, i} and Velocity{1, 0.5f} to each
        for (int i = 0; i < N; i++) {
            ecs::Entity e = w.create();
            w.add(e, Position{ static_cast<float>(i), static_cast<float>(i) });
            w.add(e, Velocity{ 1, 0.5f });
        }
        std::printf("setup, %d, %.2f ms\n", N, t.ms());
    }

    {
        Timer t;
        // run each<Position, Velocity> movement `passes` times with dt = 0.016f
        for (int i = 0; i < passes; i++) {
            w.each<Position, Velocity>([&](ecs::Entity, Position& p, const Velocity& v) {
                p.x += v.x * dt;
                p.y += v.y * dt;
                });
        }
        double ms = t.ms();
        std::printf("iterate_two, %d, %.2f ms, %.2f ns/entity\n", N, ms, ms * 1e6 / (N * (double)passes));
    }

    double checksum = 0;
    w.each<Position>([&](ecs::Entity, Position& p) { checksum += p.x; });
    std::printf("checksum %f\n", checksum);

    std::printf("\n------------------OOP----------------------\n");
    // setup: std::vector<GameObject*> objs; for i<N: objs.push_back(new GameObject{...});
    // timed: for passes: for (auto* o : objs) o->update(dt);
    // checksum, then delete them all

    {
        Timer t;

        for (int i = 0; i < N; i++) {

            GameObject* object = new GameObject();
            object->pos = Position{ static_cast<float>(i), static_cast<float>(i) };
            object->vel = Velocity{ 1, 0.5f };
            objs.push_back(object);
        }
        std::printf("setup, %d, %.2f ms\n", N, t.ms());
    }
    std::shuffle(objs.begin(), objs.end(), std::mt19937{ 42 });
    {
        Timer t;
        // run each<Position, Velocity> movement `passes` times with dt = 0.016f
        for (int i = 0; i < passes; i++) {
            for (auto* o : objs) o->update(dt);
        }
        double ms = t.ms();
        std::printf("iterate_two, %d, %.2f ms, %.2f ns/gameobject\n", N, ms, ms * 1e6 / (N * (double)passes));
        std::printf("setup, %d, %.2f ms\n", N, t.ms());
    }

    double oopSum = 0;
    for (auto* o : objs) oopSum += o->pos.x;
    std::printf("checksum %f\n", oopSum);
    for (auto* o : objs) delete o;
}