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
    long long visited;      // entities the iterate loop actually touched per pass
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
    row("Visited per pass", [](const Result& r) { std::printf("%lld", r.visited); });
    row("Setup (ms)", [](const Result& r) { std::printf("%.2f", r.setupMs); });
    row("Setup (ns/entity)", [](const Result& r) { std::printf("%.2f", r.setupMs * 1e6 / r.big); });
    row("Iterate (ms)", [](const Result& r) { std::printf("%.2f", r.iterMs); });
    row("Iterate (ns/visited)", [](const Result& r) { std::printf("%.2f", r.iterMs * 1e6 / ((double)r.visited * r.passes)); });
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
    const int smallpool = 10'000'000;
    const int passes = 100;
    const float dt = 0.016f;

    // ECS, bigger pool first: each<Position, Velocity> walks every Position
    {
        ecs::World w;
        double setupMs, iterMs;
        { Timer t; fill(w, bigpool, smallpool); setupMs = t.ms(); }
        {
            Timer t;
            for (int i = 0; i < passes; i++) {
                w.each<Position, Velocity>([&](ecs::Entity, Position& p, const Velocity& v) {
                    p.x += v.x * dt;
                    p.y += v.y * dt;
                    });
            }
            iterMs = t.ms();
        }
        results.push_back({ "ECS", bigpool, smallpool, bigpool, passes, setupMs, iterMs, checksum(w) });
    }

    // OOP, Unity-style: shuffled objects, components are separate heap objects, visits every object
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

        {
            Timer t;
            for (int i = 0; i < passes; i++)
                for (auto* o : objs) o->update(dt);
            iterMs = t.ms();
        }

        double sum = 0;
        for (auto* o : inOrder) sum += o->pos->x;
        results.push_back({ "OOP, Unity-style (shuffled ptr)", bigpool, smallpool, bigpool, passes, setupMs, iterMs, sum });

        for (auto* o : objs) { delete o->vel; delete o->pos; delete o; }
    }

    printTable();
}