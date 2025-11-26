#pragma once
#include "../ECS.h"

// Forward declarations
class TransformComponent;
class CollisionComponent;
class CameraComponent;
class TileMapComponent;
struct SDL_Renderer;

class DebugRenderSystem : public ECS::System
{
private:
    CameraComponent *camera;
    ECS::Entity *tileMapEntity;
    bool enable = false;

public:
    DebugRenderSystem(bool state = false);

    void setTileMapEntity(ECS::Entity *entity);
    
    void toggle();

    void setEnable(bool state);


    void setCamera(CameraComponent *cam) { camera = cam; }

    void renderer(SDL_Renderer* renderer);
};