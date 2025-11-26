#pragma once
#include "../ECS.h"

// Forward declarations
class TransformComponent;

class MovementSystem : public ECS::System {
public:
    MovementSystem();
    void update(float deltaTime) override;
};
