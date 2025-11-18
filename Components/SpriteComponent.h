#pragma once
#include "../ECS.h"
#include <SDL3/SDL.h>
#include <string>

/*
 * ============================================================================
 * SpriteComponent - Affichage d'images/sprites avec SDL3
 * ============================================================================
 * Gère l'affichage graphique d'une entité
 *
 * Dépend de: TransformComponent (pour la position et l'échelle)
 * Utilisé par: RenderSystem
 *
 * Contient:
 * - texture: La texture SDL à afficher
 * - srcRect: Rectangle source dans la texture (pour les spritesheets)
 * - dstRect: Rectangle de destination à l'écran (calculé automatiquement)
 *
 * Usage:
 *   entity.addComponent<SpriteComponent>("assets/player.png", 32, 32);
 *   auto& sprite = entity.getComponent<SpriteComponent>();
 *   sprite.setTexture(newTexture);
 * ============================================================================
 */

class SpriteComponent : public ECS::Component {
public:
    SDL_Texture* texture = nullptr;
    SDL_FRect srcRect;  // Source dans la texture (quelle partie afficher)
    SDL_FRect dstRect;  // Destination à l'écran (où afficher)

    int width = 0;      // Largeur du sprite dans la texture
    int height = 0;     // Hauteur du sprite dans la texture

    bool flipHorizontal = false;
    bool flipVertical = false;

    // Ordre de rendu (z-index): plus élevé = affiché par-dessus
    int renderLayer = 0;

    // ========================================================================
    // CONSTRUCTEURS
    // ========================================================================

    /*
     * Constructeur par défaut (texture sera assignée plus tard)
     */
    SpriteComponent() {
        srcRect = { 0, 0, 0, 0 };
        dstRect = { 0, 0, 0, 0 };
    }

    /*
     * Constructeur avec dimensions
     * La texture doit être chargée séparément via setTexture()
     */
    SpriteComponent(int w, int h) : width(w), height(h) {
        srcRect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
        dstRect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
    }

    /*
     * Constructeur avec texture SDL (déjà chargée)
     */
    SpriteComponent(SDL_Texture* tex, int w, int h)
        : texture(tex), width(w), height(h) {
        srcRect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
        dstRect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };
    }

    // ========================================================================
    // MÉTHODES
    // ========================================================================

    /*
     * Définit la texture à afficher
     */
    void setTexture(SDL_Texture* tex) {
        texture = tex;
    }

    /*
     * Définit quelle partie de la texture afficher (pour spritesheets)
     * Paramètres: x, y (position dans la texture), w, h (dimensions)
     */
    void setSourceRect(float x, float y, float w, float h) {
        srcRect = { x, y, w, h };
    }

    /*
     * Définit la région source par index (pour spritesheets en grille)
     * Exemple: pour un spritesheet 8x8 de tiles 32x32, index 5 = tile (5, 0)
     */
    void setSourceRectByIndex(int index, int columns, int tileWidth, int tileHeight) {
        int row = index / columns;
        int col = index % columns;
        srcRect = {
            static_cast<float>(col * tileWidth),
            static_cast<float>(row * tileHeight),
            static_cast<float>(tileWidth),
            static_cast<float>(tileHeight)
        };
    }

    void init() override {}
};