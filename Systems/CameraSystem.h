#pragma once
#include "../ECS.h"

// Forward declarations
class CameraComponent;
class TransformComponent;

class CameraSystem : public ECS::System
{

private:
    ECS::Entity *targetEntity = nullptr;

public:
    CameraSystem();

    void setTarget(ECS::Entity *entity);

    void update(float deltaTime) override;

private:
    float clamp(float value, float min, float max);
};