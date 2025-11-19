#pragma once
#include "../ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <SDL2/SDL.h>

/*
 * ============================================================================
 * AnimationSystem - Met a jour les animations des sprites
 * ============================================================================
 * Ce systeme met a jour les animations de toutes les entités qui ont:
 * - SpriteComponent (pour changer le srcRect)
 * - AnimationComponent (pour connaitre l'animation actuelle)
 *
 * Fonctionnement:
 * 1. Verifie si assez de temps s'est écoule pour changer de frame
 * 2. Incrémente la frame actuelle
 * 3. Boucle ou arrète selon la config de l'animation
 * 4. Met a jour le srcRect du SpriteComponent
 *
 * Usage:
 *   AnimationSystem* animSys = manager.addSystem<AnimationSystem>(tileWidth, tileHeight);
 *   animSys->setPriority(20);  // Avant le RenderSystem
 * ============================================================================
 */

class AnimationSystem : public ECS::System {
private:
    int tileWidth;   // Largeur d'une frame dans le spritesheet
    int tileHeight;  // Hauteur d'une frame dans le spritesheet

public:
    /*
     * Constructeur
     *
     * Paramètres:
     * - tileW: Largeur d'une frame/tile dans votre spritesheet
     * - tileH: Hauteur d'une frame/tile dans votre spritesheet
     *
     * Exemple: Si vos sprites font 32x32 pixels, passez (32, 32)
     */
    AnimationSystem(int tileW, int tileH)
        : tileWidth(tileW), tileHeight(tileH) {
        // Ce système requiert SpriteComponent et AnimationComponent
        requireComponent<SpriteComponent>();
        requireComponent<AnimationComponent>();
    }

    void init() override {
        std::cout << "[AnimationSystem] Initialized with tile size: "
            << tileWidth << "x" << tileHeight << "\n";
    }

    /*
     * Met � jour toutes les animations
     * deltaTime n'est pas utilis� car on utilise SDL_GetTicks() pour le timing
     */
    void update(float deltaTime) override {
        Uint64 currentTime = SDL_GetTicks();

        // Parcourir toutes les entit�s avec Sprite + Animation
        for (auto entity : getEntities()) {
            auto& sprite = entity->getComponent<SpriteComponent>();
            auto& anim = entity->getComponent<AnimationComponent>();

            // Si l'animation est en pause, skip
            if (!anim.isPlaying) {
                continue;
            }

            // V�rifier si l'animation actuelle existe
            if (anim.animations.find(anim.currentAnimState) == anim.animations.end()) {
                std::cerr << "[AnimationSystem] Animation '"
                    << anim.currentAnimState << "' not found!\n";
                continue;
            }

            const Animation& currentAnim = anim.animations[anim.currentAnimState];

            // V�rifier si assez de temps s'est �coul� pour changer de frame
            Uint64 elapsed = currentTime - anim.lastFrameTime;

            if (elapsed >= static_cast<Uint64>(currentAnim.speed)) {
                // Temps de changer de frame!
                anim.currentFrame++;

                // Gestion de la boucle ou de l'arr�t
                if (anim.currentFrame >= currentAnim.frames) {
                    if (currentAnim.loop) {
                        // Boucler l'animation
                        anim.currentFrame = 0;
                    }
                    else {
                        // Arr�ter � la derni�re frame
                        anim.currentFrame = currentAnim.frames - 1;
                        anim.isPlaying = false;
                    }
                }

                // Mettre � jour le temps du dernier changement
                anim.lastFrameTime = currentTime;
            }

            // Mettre � jour le srcRect du sprite selon la frame actuelle
            updateSpriteRect(sprite, anim, currentAnim);
        }
    }

private:
    /*
     * Met � jour le rectangle source du sprite
     * Calcule la position dans le spritesheet selon l'animation et la frame
     */
    void updateSpriteRect(SpriteComponent& sprite,
        const AnimationComponent& anim,
        const Animation& currentAnim) {
        // Calculer la position dans le spritesheet
        // - Row (y): D�pend de l'index de l'animation (quelle ligne)
        // - Col (x): D�pend de la frame actuelle (quelle colonne)

        int row = currentAnim.index;
        int col = anim.currentFrame;

        sprite.srcRect.x = static_cast<float>(col * tileWidth);
        sprite.srcRect.y = static_cast<float>(row * tileHeight);
        sprite.srcRect.w = static_cast<float>(tileWidth);
        sprite.srcRect.h = static_cast<float>(tileHeight);
    }
};