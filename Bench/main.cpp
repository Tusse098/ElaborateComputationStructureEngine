// bench/main.cpp
#include <ecs/core/world.h>
#include <cstdio>
#include <vector>
#include <bench/core/oop.h>
#include <bench/core/timer.h>
#include <bench/core/components.h>
#include <algorithm>
#include <random>

// One column per benchmark, one row per metric. Printed as a markdown table at the end.
struct Result {
    const char* name;
    int big, small;
    long long perPass;      // unit of work per pass: entities visited, or entities churned
    int passes;
    double setupMs, iterMs, checksum;
};

static std::vector<Result> results;

static void printTable() {
    auto row = [](const char* label, auto value) {
        std::printf("| %s |", label);
        for (const Result& r : results) { std::printf(" "); value(r); std::printf(" |"); }
        std::printf("\n");
        };

    std::printf("| Metric |");
    for (const Result& r : results) std::printf(" %s |", r.name);
    std::printf("\n| --- |");
    for (size_t i = 0; i < results.size(); i++) std::printf(" ---: |");
    std::printf("\n");

    row("Entities (big pool)", [](const Result& r) { std::printf("%d", r.big); });
    row("With Velocity (small)", [](const Result& r) { std::printf("%d", r.small); });
    row("Churned per pass", [](const Result& r) { std::printf("%lld", r.perPass); });
    row("Setup (ms)", [](const Result& r) { std::printf("%.2f", r.setupMs); });
    row("Setup (ns/entity)", [](const Result& r) { std::printf("%.2f", r.setupMs * 1e6 / r.big); });
    row("Iterate (ms)", [](const Result& r) { std::printf("%.2f", r.iterMs); });
    row("ns / entity churned", [](const Result& r) { std::printf("%.2f", r.iterMs * 1e6 / ((double)r.perPass * r.passes)); });
    row("Checksum", [](const Result& r) { std::printf("%.6f", r.checksum); });
}

static void fill(ecs::World& w, int big, int small) {
    for (int i = 0; i < big; i++) {
        ecs::Entity e = w.create();
        w.add(e, Position{ static_cast<float>(i), static_cast<float>(i) });
        if (i < small) w.add(e, Velocity{ 1, 0.5f });
    }
}

static double checksum(ecs::World& w) {
    double sum = 0;
    w.each<Position>([&](ecs::Entity, Position& p) { sum += p.x; });
    return sum;
}

int main() {
    const int bigpool = 10'000'000;
    const int smallpool = 1'000;
    const int passes = 100;

    // ECS, churn via ECB: destroy every entity with Velocity and respawn it, all through the buffer
    {
        ecs::World w;
        double setupMs, iterMs;
        { Timer t; fill(w, bigpool, smallpool); setupMs = t.ms(); }
        {
            Timer t;
            for (int i = 0; i < passes; i++) {
                w.each<Position, Velocity>([&](ecs::Entity e, Position&, Velocity&) {
                    w.defer().destroy(e);
                    ecs::Entity n = w.create();                  // immediate: never touches a pool
                    w.defer().add(n, Position{ 0, 0 });
                    w.defer().add(n, Velocity{ 1, 0.5f });
                    });
                w.flush();
            }
            iterMs = t.ms();
        }
        results.push_back({ "ECS, churn via ECB", bigpool, smallpool, smallpool, passes, setupMs, iterMs, checksum(w) });
    }

    // ECS, churn direct: same work, no buffer. Collect inside each, apply after it.
    // Control column: ECB minus this is the buffer's own cost.
    {
        ecs::World w;
        double setupMs, iterMs;
        { Timer t; fill(w, bigpool, smallpool); setupMs = t.ms(); }
        std::vector<ecs::Entity> dead;
        dead.reserve(smallpool);
        {
            Timer t;
            for (int i = 0; i < passes; i++) {
                dead.clear();
                w.each<Position, Velocity>([&](ecs::Entity e, Position&, Velocity&) {
                    dead.push_back(e);
                    });
                for (ecs::Entity e : dead) {
                    w.destroy(e);
                    ecs::Entity n = w.create();
                    w.add(n, Position{ 0, 0 });
                    w.add(n, Velocity{ 1, 0.5f });
                }
            }
            iterMs = t.ms();
        }
        results.push_back({ "ECS, churn direct", bigpool, smallpool, smallpool, passes, setupMs, iterMs, checksum(w) });
    }

    // OOP, churn: scan every object for a Velocity, then delete and new both components on each hit
    {
        std::vector<GameObject*> objs;
        objs.reserve(bigpool);
        double setupMs, iterMs;
        {
            Timer t;
            for (int i = 0; i < bigpool; i++) {
                GameObject* object = new GameObject();
                object->pos = new Position{ static_cast<float>(i), static_cast<float>(i) };
                if (i < smallpool) object->vel = new Velocity{ 1, 0.5f };
                objs.push_back(object);
            }
            setupMs = t.ms();
        }

        std::vector<GameObject*> inOrder = objs;                    // for an order-independent checksum
        std::shuffle(objs.begin(), objs.end(), std::mt19937{ 42 });

        std::vector<GameObject*> dead;
        dead.reserve(smallpool);
        {
            Timer t;
            for (int i = 0; i < passes; i++) {
                dead.clear();
                for (auto* o : objs) if (o->vel) dead.push_back(o);   // the query: scan all 10M
                for (auto* o : dead) {                                 // the flush: kill + respawn
                    delete o->vel; delete o->pos;
                    o->pos = new Position{ 0, 0 };
                    o->vel = new Velocity{ 1, 0.5f };
                }
            }
            iterMs = t.ms();
        }

        double sum = 0;
        for (auto* o : inOrder) sum += o->pos->x;
        results.push_back({ "OOP, churn", bigpool, smallpool, smallpool, passes, setupMs, iterMs, sum });

        for (auto* o : objs) { delete o->vel; delete o->pos; delete o; }
    }

    printTable();
}