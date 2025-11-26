#include "MovementSystem.h"
#include "../Components/TransformComponent.h"

MovementSystem::MovementSystem()
{
    requireComponent<TransformComponent>();
}

void MovementSystem::update(float deltaTime)
{
    for (auto entity : getEntities())
    {
        auto &transform = entity->getComponent<TransformComponent>();
        transform.position.x += transform.velocity.x * deltaTime;
        transform.position.y += transform.velocity.y * deltaTime;
    }
}
