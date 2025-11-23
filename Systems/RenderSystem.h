#pragma once
#include "../ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/CameraComponent.h"
#include <SDL2/SDL.h>
#include <algorithm>

class RenderSystem : public ECS::System
{
private:
    SDL_Renderer *renderer = nullptr;
    CameraComponent *camera = nullptr;

public:
    RenderSystem(SDL_Renderer *rend) : renderer(rend)
    {
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void setCamera(CameraComponent *cam) { camera = cam; }

    void init() override
    {
        std::cout << "[RenderSystem] Initialized\n";
    }

    void update(float deltaTime) override
    {
        (void)deltaTime; // Éviter warning unused parameter


        
        std::vector<ECS::Entity *> sortedEntities = getEntities();

        // Tri par renderLayer
        std::sort(sortedEntities.begin(), sortedEntities.end(),
                  [](ECS::Entity *a, ECS::Entity *b)
                  {
                      auto &spriteA = a->getComponent<SpriteComponent>();
                      auto &spriteB = b->getComponent<SpriteComponent>();
                      return spriteA.renderLayer < spriteB.renderLayer;
                  });

        // Dessiner chaque entité
        for (auto entity : sortedEntities)
        {
            auto &transform = entity->getComponent<TransformComponent>();
            auto &sprite = entity->getComponent<SpriteComponent>();

            if (sprite.texture == nullptr)
                continue;

            float zoom = camera ? camera->zoom : 1.0f;

            float worldX = transform.position.x;
            float worldY = transform.position.y;

            float screenX = transform.position.x;
            float screenY = transform.position.y;

            if (camera)
            {
                screenX = (worldX - camera->position.x) * zoom;
                screenY = (worldY - camera->position.y) * zoom;
            }


            sprite.dstRect.x = static_cast<int>(screenX);
            sprite.dstRect.y = static_cast<int>(screenY);
            sprite.dstRect.w = static_cast<int>(sprite.srcRect.w * transform.scale * zoom); // ✅ Zoom
            sprite.dstRect.h = static_cast<int>(sprite.srcRect.h * transform.scale * zoom);

            // Définir le flip
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (sprite.flipHorizontal && sprite.flipVertical)
            {
                flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else if (sprite.flipHorizontal)
            {
                flip = SDL_FLIP_HORIZONTAL;
            }
            else if (sprite.flipVertical)
            {
                flip = SDL_FLIP_VERTICAL;
            }

            // Dessiner avec rotation
            SDL_Point center = {
                sprite.dstRect.w / 2,
                sprite.dstRect.h / 2};

            SDL_RenderCopyEx(
                renderer,
                sprite.texture,
                &sprite.srcRect,
                &sprite.dstRect,
                transform.rotation,
                &center,
                flip);
        }
    }

    void onEntityAdded(ECS::Entity *entity) override
    {
        (void)entity; // Éviter warning
    }

    void onEntityRemoved(ECS::Entity *entity) override
    {
        (void)entity; // Éviter warning
    }
};