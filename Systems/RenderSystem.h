#pragma once
#include "../ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL3/SDL.h>
#include <algorithm>

/*
 * ============================================================================
 * RenderSystem - Gère l'affichage de toutes les entités avec sprites
 * ============================================================================
 * Ce système dessine toutes les entités qui ont à la fois:
 * - TransformComponent (pour la position)
 * - SpriteComponent (pour la texture)
 *
 * Fonctionnalités:
 * - Tri par renderLayer (z-index)
 * - Support du flip horizontal/vertical
 * - Respect de l'échelle et de la rotation
 *
 * Usage:
 *   RenderSystem* renderSys = manager.addSystem<RenderSystem>(renderer);
 *   renderSys->setPriority(100);  // Rendu après les autres systèmes
 * ============================================================================
 */

class RenderSystem : public ECS::System {
private:
    SDL_Renderer* renderer = nullptr;

public:
    /*
     * Constructeur: nécessite un renderer SDL3
     */
    RenderSystem(SDL_Renderer* rend) : renderer(rend) {
        // Ce système requiert TransformComponent et SpriteComponent
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void init() override {
        std::cout << "[RenderSystem] Initialized\n";
    }

    /*
     * update() est appelé chaque frame
     * deltaTime n'est pas utilisé ici car le rendu ne dépend pas du temps
     */
    void update(float deltaTime) override {
        // Récupérer toutes les entités avec Transform + Sprite
        std::vector<ECS::Entity*> sortedEntities = getEntities();

        // Tri par renderLayer (du plus bas au plus haut = arrière vers avant)
        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](ECS::Entity* a, ECS::Entity* b) {
                auto& spriteA = a->getComponent<SpriteComponent>();
                auto& spriteB = b->getComponent<SpriteComponent>();
                return spriteA.renderLayer < spriteB.renderLayer;
            });

        // Dessiner chaque entité
        for (auto entity : sortedEntities) {
            auto& transform = entity->getComponent<TransformComponent>();
            auto& sprite = entity->getComponent<SpriteComponent>();

            // Si pas de texture, on skip
            if (sprite.texture == nullptr) continue;

            // Calculer le rectangle de destination basé sur le transform
            sprite.dstRect.x = transform.position.x;
            sprite.dstRect.y = transform.position.y;
            sprite.dstRect.w = sprite.srcRect.w * transform.scale;
            sprite.dstRect.h = sprite.srcRect.h * transform.scale;

            // Définir le flip
            SDL_FlipMode flip = SDL_FLIP_NONE;
            if (sprite.flipHorizontal && sprite.flipVertical) {
                flip = (SDL_FlipMode) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            }
            else if (sprite.flipHorizontal) {
                flip = SDL_FLIP_HORIZONTAL;
            }
            else if (sprite.flipVertical) {
                flip = SDL_FLIP_VERTICAL;
            }

            // Dessiner avec rotation
            // Le centre de rotation est au centre du sprite
            SDL_FPoint center = {
                sprite.dstRect.w / 2.0f,
                sprite.dstRect.h / 2.0f
            };

            SDL_RenderTextureRotated(
                renderer,
                sprite.texture,
                &sprite.srcRect,    // Source
                &sprite.dstRect,    // Destination
                transform.rotation, // Angle en degrés
                &center,            // Centre de rotation
                flip                // Flip horizontal/vertical
            );
        }
    }

    /*
     * Hook appelé quand une entité matche ce système
     */
    void onEntityAdded(ECS::Entity* entity) override {
        // Optionnel: logique d'initialisation quand une entité est ajoutée
    }

    /*
     * Hook appelé quand une entité ne matche plus ce système
     */
    void onEntityRemoved(ECS::Entity* entity) override {
        // Optionnel: nettoyage si nécessaire
    }
}; 