#pragma once
#include "components.h"

struct GameObject {
    virtual ~GameObject() = default;
    Position pos; Velocity vel;
    virtual void update(float dt) { pos.x += vel.x * dt; pos.y += vel.y * dt; }
};
