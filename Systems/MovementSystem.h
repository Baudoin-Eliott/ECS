#pragma once

#include "../ECS.h"
#include "../Components/TransformComponent.h"



class MovementSystem : public ECS::System
{

public:
    MovementSystem()
    {
        requireComponent<TransformComponent>();
    }

    void init() override
    {
        std::cout << "[MovementSystem] Initialized\n";
    }

    void update(float deltaTime) override
    {

        for (auto entity : getEntities())
        {
            auto &transform = entity->getComponent<TransformComponent>();
            auto &sprite = entity->getComponent<SpriteComponent>();
            transform.position.x += transform.velocity.x * deltaTime;
            transform.position.y += transform.velocity.y * deltaTime;
        }
    }
};
