#pragma once
#include "../ECS.h"
#include "../Utils/Vector2D.h"



class TransformComponent : public ECS::Component {
public:
    Vector2D position;
    Vector2D velocity;
    float scale;
    float rotation;  

    TransformComponent()
        : position(0, 0), velocity(0, 0), scale(1.0f), rotation(0.0f) {}


    TransformComponent(float x, float y)
        : position(x, y), velocity(0, 0), scale(1.0f), rotation(0.0f) {}

    TransformComponent(float x, float y, float s)
        : position(x, y), velocity(0, 0), scale(s), rotation(0.0f) {}


    TransformComponent(float x, float y, float s, float rot)
        : position(x, y), velocity(0, 0), scale(s), rotation(rot) {}

};