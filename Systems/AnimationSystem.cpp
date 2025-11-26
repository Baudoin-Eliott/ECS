#include "AnimationSystem.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <SDL2/SDL.h>
#include <iostream>

AnimationSystem::AnimationSystem(int tileW, int tileH)
    : tileWidth(tileW), tileHeight(tileH)
{

    requireComponent<SpriteComponent>();
    requireComponent<AnimationComponent>();
}

void AnimationSystem::update(float deltaTime) override
{
    Uint64 currentTime = SDL_GetTicks();

    for (auto entity : getEntities())
    {
        auto &sprite = entity->getComponent<SpriteComponent>();
        auto &anim = entity->getComponent<AnimationComponent>();

        if (!anim.isPlaying)
        {
            continue;
        }

        if (anim.animations.find(anim.currentAnimState) == anim.animations.end())
        {
            std::cerr << "[AnimationSystem] Animation '"
                      << anim.currentAnimState << "' not found!\n";
            continue;
        }

        const Animation &currentAnim = anim.animations[anim.currentAnimState];

        Uint64 elapsed = currentTime - anim.lastFrameTime;

        if (elapsed >= static_cast<Uint64>(currentAnim.speed))
        {

            anim.currentFrame++;

            if (anim.currentFrame >= currentAnim.frames)
            {
                if (currentAnim.loop)
                {

                    anim.currentFrame = 0;
                }
                else
                {

                    anim.currentFrame = currentAnim.frames - 1;
                    anim.isPlaying = false;
                }
            }

            anim.lastFrameTime = currentTime;
        }

        updateSpriteRect(sprite, anim, currentAnim);
    }
}

void AnimationSystem::updateSpriteRect(SpriteComponent &sprite, const AnimationComponent &anim, const Animation &currentAnim)
{

    int row = currentAnim.index;
    int col = anim.currentFrame;

    sprite.srcRect.x = static_cast<float>(col * tileWidth);
    sprite.srcRect.y = static_cast<float>(row * tileHeight);
    sprite.srcRect.w = static_cast<float>(tileWidth);
    sprite.srcRect.h = static_cast<float>(tileHeight);
}
