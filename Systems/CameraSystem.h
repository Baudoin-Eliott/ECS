#pragma once

#include "../ECS.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../../src/Components/PlayerComponent.h"

class CameraSystem : public ECS::System
{

private:
    ECS::Entity *targetEntity = nullptr;

public:
    CameraSystem()
    {
        requireComponent<CameraComponent>();
    }

    void init() override
    {
        std::cout << "[CameraSystem] Initialized " << std::endl;
    }

    void setTarget(ECS::Entity *entity)
    {
        targetEntity = entity;
        std::cout << "[CameraSystem] target set to entity ID :" << entity->getID() << std::endl;
    }

    void update(float deltaTime) override
    {
        (void)deltaTime;

        for (auto cameraEntity : getEntities())
        {
            auto &camera = cameraEntity->getComponent<CameraComponent>();

            if (targetEntity && targetEntity->hasComponent<TransformComponent>())
            {
                auto &transform = targetEntity->getComponent<TransformComponent>();

                float targetX = transform.position.x;
                float targetY = transform.position.y;

                
                float visibleWorldWidth = camera.viewportWidth / camera.zoom;
                float visibleWorldHeight = camera.viewportHeight / camera.zoom;

               
                camera.position.x = targetX - (visibleWorldWidth / 2.0f);
                camera.position.y = targetY - (visibleWorldHeight / 2.0f);

                
                float maxPosX = camera.maxX - visibleWorldWidth;
                float maxPosY = camera.maxY - visibleWorldHeight;

                camera.position.x = clamp(camera.position.x, camera.minX, maxPosX);
                camera.position.y = clamp(camera.position.y, camera.minY, maxPosY);
            }
        }
    }

private:
    float clamp(float value, float min, float max)
    {
        return value < min ? min : value > max ? max
                                               : value;
    }
};