#pragma once
#include "../ECS.h"
#include <functional>
#include <set>
#include <string>

// Forward declarations
class CollisionComponent;
class TransformComponent;
class TileMapComponent;
class PlayerComponent;
struct TiledObject;

class TriggerSystem : public ECS::System
{
private:
    ECS::Entity *tileMapEntity;
    std::set<TiledObject *> triggeredObjects;
    std::function<void(const std::string &, const std::string &)> onTeleportCallback;

public:
    TriggerSystem();

    void setTileMapEntity(ECS::Entity *entity) { tileMapEntity = entity; }

    void setTeleportCallback(std::function<void(const std::string &, const std::string &)> callback);

    void update(float deltaTime) override;

    void onTriggerEnter(TiledObject *trigger);
};