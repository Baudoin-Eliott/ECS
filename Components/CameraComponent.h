#pragma once
#include "../ECS.h"
#include "../Utils/Vector2D.h"
#include "SDL2/SDL.h"

class CameraComponent : public ECS::Component
{
public:
    Vector2D position;

    float viewportWidth;
    float viewportHeight;

    float zoom;

    float minX, maxX, minY, maxY;

    CameraComponent()
        : position(0, 0),
          viewportWidth(800),
          viewportHeight(600),
          zoom(1.0f),
          minX(0), maxX(0), minY(0), maxY(0) {}

    CameraComponent(float vpWidth, float vpHeight)
        : position(0, 0),
          viewportWidth(vpWidth),
          viewportHeight(vpHeight),
          zoom(1.0f),
          minX(0), maxX(0), minY(0), maxY(0) {}

    void setBounds(float p_minX, float p_maxX, float p_minY, float p_maxY)
    {
        minX = p_minX; 
        maxX = p_maxX;
        minY = p_minY;
        maxY = p_maxY;
    }

    SDL_Rect getViewport() const {
        SDL_Rect rect;
        rect.x = position.x;
        rect.y = position.y;
        rect.w = viewportWidth / zoom;
        rect.h = viewportHeight / zoom;
        return rect;
    }

};