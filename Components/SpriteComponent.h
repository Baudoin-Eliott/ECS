#pragma once
#include "../ECS.h"
#include <SDL2/SDL.h>
#include <string>

class SpriteComponent : public ECS::Component {
public:
    SDL_Texture* texture = nullptr;
    SDL_Rect srcRect;  // Source dans la texture
    SDL_Rect dstRect;  // Destination à l'écran
    
    int width = 0;
    int height = 0;
    
    bool flipHorizontal = false;
    bool flipVertical = false;
    
    int renderLayer = 0;

    SpriteComponent() {
        srcRect = { 0, 0, 0, 0 };
        dstRect = { 0, 0, 0, 0 };
    }

    SpriteComponent(int w, int h) : width(w), height(h) {
        srcRect = { 0, 0, w, h };
        dstRect = { 0, 0, w, h };
    }

    SpriteComponent(SDL_Texture* tex, int w, int h) 
        : texture(tex), width(w), height(h) {
        srcRect = { 0, 0, w, h };
        dstRect = { 0, 0, w, h };
    }

    void setTexture(SDL_Texture* tex) {
        texture = tex;
    }

    void setSourceRect(int x, int y, int w, int h) {
        srcRect = { x, y, w, h };
    }

    void setSourceRectByIndex(int index, int columns, int tileWidth, int tileHeight) {
        int row = index / columns;
        int col = index % columns;
        srcRect = { 
            col * tileWidth, 
            row * tileHeight,
            tileWidth, 
            tileHeight 
        };
    }

    void init() override {}
};