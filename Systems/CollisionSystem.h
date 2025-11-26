#pragma once
#include "../ECS.h"

// Forward declarations
class TransformComponent;
class CollisionComponent;
class TileMapComponent;

class CollisionSystem : public ECS::System
{

private:
    ECS::Entity *tileMapEntity = nullptr;

public:
    CollisionSystem();

    void init() override;


    void setTileMapEntity(ECS::Entity *entity);

    void update(float deltaTime) override;
    
};