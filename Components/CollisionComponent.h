#pragma once
#include "../ECS.h"
#include "../Utils/vector2D.h"

class CollisionComponent : public ECS::Component
{
public:
    Vector2D offset;
    float width, height;
    std::string tag;

    CollisionComponent()
        : offset(0, 0), width(0), height(0), tag("default") {}

    CollisionComponent(float w, float h)
        : offset(0, 0), width(w), height(h), tag("default") {}

    CollisionComponent(float offsetX, float offsetY, float w, float h, const std::string &collisionTag = "default")
        : offset(offsetX, offsetY), width(w), height(h), tag(collisionTag) {}

    SDL_FRect getRect(const Vector2D& entityPosition) const
    {
        SDL_FRect rect = {};
        rect.x = entityPosition.x + offset.x;
        rect.y = entityPosition.y + offset.y;
        rect.w = width;
        rect.h = height;
        return rect;
    }

    bool intersects(const SDL_FRect& other, const Vector2D& pos) const
    {
        SDL_FRect thisRect = getRect(pos);

        if (thisRect.x > other.x + other.w) return false;
        if (thisRect.x + thisRect.w < other.x) return false;
        if (thisRect.y + thisRect.h < other.y) return false;
        if (thisRect.y > other.y + other.h) return false;
        
        return true;
    }

};