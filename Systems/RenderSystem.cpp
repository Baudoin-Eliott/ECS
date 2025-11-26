#include "RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/CameraComponent.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

RenderSystem::RenderSystem()
{
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void RenderSystem::render(SDL_Renderer *renderer)
    {
        std::vector<ECS::Entity *> sortedEntities = getEntities();

        std::sort(sortedEntities.begin(), sortedEntities.end(),
                  [](ECS::Entity *a, ECS::Entity *b)
                  {
                      auto &spriteA = a->getComponent<SpriteComponent>();
                      auto &spriteB = b->getComponent<SpriteComponent>();
                      return spriteA.renderLayer < spriteB.renderLayer;
                  });

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
