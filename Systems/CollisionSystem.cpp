#include "CollisionSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/TileMapComponent.h"
#include <SDL2/SDL.h>
#include <vector>

CollisionSystem::CollisionSystem()
{
    requireComponent<CollisionComponent>();
    requireComponent<TransformComponent>();
}

void CollisionSystem::init()
{
    for (auto &entity : manager->getEntities())
    {
        if (entity->hasComponent<TileMapComponent>())
        {
            tileMapEntity = entity.get();
        }
    }
}

void CollisionSystem::setTileMapEntity(ECS::Entity *entity)
{
    tileMapEntity = entity;
}

void CollisionSystem::update(float deltaTime)
{

    if (!tileMapEntity)
        return;

    auto &tileMapComp = tileMapEntity->getComponent<TileMapComponent>();

    std::vector<TiledObject *> collisions = tileMapComp.getObjectsByGroup("Collision");

    for (auto &entity : getEntities())
    {

        auto &transform = entity->getComponent<TransformComponent>();
        auto &collision = entity->getComponent<CollisionComponent>();

        float originalSpeed = transform.velocity.Magnitude();

        float futurePosX = transform.position.x + transform.velocity.x * deltaTime;
        float futurePosY = transform.position.y + transform.velocity.y * deltaTime;
        for (auto &col : collisions)
        {
            SDL_FRect colRect = {col->x, col->y, col->width, col->height};
            if (collision.intersects(colRect, {futurePosX, transform.position.y}))
            {
                transform.velocity.x = 0;
                if (transform.velocity.y != 0)
                {
                    float dirY = transform.velocity.y > 0 ? 1.0f : -1.0f;
                    transform.velocity.y = dirY * originalSpeed;
                }
            }
            if (collision.intersects(colRect, {transform.position.x, futurePosY}))
            {
                transform.velocity.y = 0;
                if (transform.velocity.x != 0)
                {
                    float dirX = transform.velocity.x > 0 ? 1.0f : -1.0f;
                    transform.velocity.x = dirX * originalSpeed;
                }
            }
        }
    }
}
