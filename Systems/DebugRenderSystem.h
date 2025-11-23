#pragma once
#include "../ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/TileMapComponent.h"
#include <SDL2/SDL.h>

class DebugRenderSystem : public ECS::System
{
private:
    CameraComponent *camera;
    SDL_Renderer *renderer;
    ECS::Entity *tileMapEntity;
    bool enable = false;

public:
    DebugRenderSystem(SDL_Renderer *rend, bool state = false) : renderer(rend), enable(state)
    {
        requireComponent<TransformComponent>();
        requireComponent<CollisionComponent>();
    }

    void setTileMapEntity(ECS::Entity *entity)
    {
        tileMapEntity = entity;
        std::cout << "[DebugRenderSystem] TileMap entity set\n";
    }
    void toggle()
    {
        enable = !enable;
        std::cout << "[DebugRenderSystem] " << (enable ? "Enabled" : "Disabled") << "\n";
    }

    void setEnable(bool state)
    {
        enable = state;
        std::cout << "[DebugRenderSystem] " << (enable ? "Enabled" : "Disabled") << "\n";
    }

    void setCamera(CameraComponent *cam) { camera = cam; }

    void init() override
    {
        std::cout << "[DebugRenderSystem] Initialized\n";
    }

    void update(float deltaTime)
    {

        (void)deltaTime;

        if (!camera)
            return;
        if (!enable)
            return;

        for (auto &entity : getEntities())
        {

            auto &transform = entity->getComponent<TransformComponent>();
            auto &collider = entity->getComponent<CollisionComponent>();

            SDL_FRect rect = collider.getRect(transform.position);

            float screenX = (rect.x - camera->position.x) * camera->zoom;
            float screenY = (rect.y - camera->position.y) * camera->zoom;
            float screenW = rect.w * camera->zoom;
            float screenH = rect.h * camera->zoom;

            SDL_FRect screenRect = {screenX, screenY, screenW, screenH};

            if (collider.tag == "Player")
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

            if (collider.tag == "Enemy")
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            if (collider.tag == "Spell")
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

            SDL_RenderDrawRectF(renderer, &screenRect);
        }
        if (tileMapEntity && tileMapEntity->hasComponent<TileMapComponent>())
        {
            auto &tileMapComp = tileMapEntity->getComponent<TileMapComponent>();

            std::vector<TiledObject*> collisions = tileMapComp.getObjectsByGroup("Collision");

            for (auto& wall : collisions)
            {
                SDL_FRect worldRect = {wall->x, wall->y, wall->width, wall->height};

                float screenX = (worldRect.x - camera->position.x) * camera->zoom;
                float screenY = (worldRect.y - camera->position.y) * camera->zoom;
                float screenW = worldRect.w * camera->zoom;
                float screenH = worldRect.h * camera->zoom;

                SDL_FRect screenRect = {screenX, screenY, screenW, screenH};

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRectF(renderer, &screenRect);
            }

            std::vector<TiledObject*> triggers = tileMapComp.getObjectsByGroup("Triggers");


            for(auto& trigger : triggers){
                SDL_FRect worldRect = {trigger->x, trigger->y, trigger->width, trigger->height};

                float screenX = (worldRect.x - camera->position.x) * camera->zoom;
                float screenY = (worldRect.y - camera->position.y) * camera->zoom;
                float screenW = worldRect.w * camera->zoom;
                float screenH = worldRect.h * camera->zoom;

                SDL_FRect screenRect = {screenX, screenY, screenW, screenH};

                SDL_SetRenderDrawColor(renderer, 225, 20, 247, 255);
                SDL_RenderDrawRectF(renderer, &screenRect);
            }
        }
    }
};