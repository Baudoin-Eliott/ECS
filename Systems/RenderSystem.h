#pragma once
#include "../ECS.h"

// Forward declarations
class CameraComponent;
class TransformComponent;
class SpriteComponent;
struct SDL_Renderer;

class RenderSystem : public ECS::System
{
private:
    CameraComponent *camera = nullptr;

public:
    RenderSystem();

    void setCamera(CameraComponent *cam) { camera = cam; }

    void render(SDL_Renderer* renderer) override;
};