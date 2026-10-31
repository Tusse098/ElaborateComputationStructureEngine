#pragma once
#include "components.h"

struct GameObject {
    virtual ~GameObject() = default;
    Position* pos = nullptr;    // always present, like Transform
    Velocity* vel = nullptr;    // optional, like Rigidbody
    virtual void update(float dt) {
        if (!vel) return;
        pos->x += vel->x * dt;
        pos->y += vel->y * dt;
    }
};
