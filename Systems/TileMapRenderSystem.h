#pragma once
#include "../ECS.h"

// Forward declarations
class TileMapComponent;
class CameraComponent;
struct Layer;
struct SDL_Renderer;

class TileMapRenderSystem : public ECS::System
{

private:
    CameraComponent *camera = nullptr;
    int targetRenderOrder;

public:
    void setCamera(CameraComponent *cam) { camera = cam; }
    TileMapRenderSystem(int renderOrder = 0);

    void render(SDL_Renderer *renderer) override;

private:
    void drawLayer(TileMapComponent &tilemap, const Layer *layer, SDL_Renderer *renderer);
};