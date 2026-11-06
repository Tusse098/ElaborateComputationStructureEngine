// src/debugging/debug.cpp
#include <ecs/debugging/debug.h>
#include <ecs/core/world.h>

#include <cstdio>

namespace ecs::debug {

    std::string toString(Entity e) {
        if (e == null) return "entity(null)";
        return "entity(" + std::to_string(index(e)) + ", gen " + std::to_string(generation(e)) + ")";
    }

    std::string describe(const World& w, Entity e) {
        std::string s = toString(e) + (w.alive(e) ? "  alive" : "  DEAD");
        w.forEachPool([&](const IPool& p) {
            if (p.has(e)) s += "\n    " + std::string(p.name());
            });
        return s;
    }

    std::string summary(const World& w) {
        std::string s;
        w.forEachPool([&](const IPool& p) {
            s += std::string(p.name()) + ": " + std::to_string(p.size()) + "\n";
            });
        return s.empty() ? "(no pools)\n" : s;
    }

    bool validate(const World& w) {
        bool ok = true;
        w.forEachPool([&](const IPool& p) {
            if (!p.validate()) {
                std::printf("[ecs::debug] pool %s failed validation\n", p.name());
                ok = false;
            }
            });
        return ok;
    }

    void print(const World& w, Entity e) {
        std::printf("%s\n", describe(w, e).c_str());
    }

    void printSummary(const World& w) {
        std::printf("%s", summary(w).c_str());
    }

}