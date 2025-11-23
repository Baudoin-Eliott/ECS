#pragma once
#include "../ECS.h"
#include "../Components/CollisionComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TileMapComponent.h"
#include "../../src/Components/PlayerComponent.h"
#include <functional>

class TriggerSystem : public ECS::System
{
private:
    ECS::Entity *tileMapEntity;
    std::set<TiledObject *> triggeredObjects;
    std::function<void(const std::string&, const std::string&)> onTeleportCallback;


public:
    TriggerSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<CollisionComponent>();
    }

    void setTileMapEntity(ECS::Entity *entity) { tileMapEntity = entity; }

    void setTeleportCallback(std::function<void(const std::string&, const std::string&)> callback) {
        onTeleportCallback = callback;
        std::cout << "[TriggerSystem] Teleport callback set\n";
    }

    void init() override
    {
        std::cout << "[TriggerSystem] initialized\n";
    }

    void update(float deltaTime) override
    {
        (void)deltaTime;

        if (!tileMapEntity)
            return;

        for (auto &entity : getEntities())
        {

            if (entity->hasComponent<PlayerComponent>())
            {
                auto &transform = entity->getComponent<TransformComponent>();
                auto &collision = entity->getComponent<CollisionComponent>();

                std::vector<TiledObject *> triggers = tileMapEntity->getComponent<TileMapComponent>().getObjectsByGroup("Triggers");

                for (auto &trigger : triggers)
                {
                    SDL_FRect triggerRect = {trigger->x, trigger->y, trigger->width, trigger->height};
                    if (collision.intersects(triggerRect, transform.position))
                    {
                       
                        if (triggeredObjects.find(trigger) == triggeredObjects.end())
                        {
                            onTriggerEnter(trigger);
                            triggeredObjects.insert(trigger);
                        }
                    }
                    else
                    {
                        
                        triggeredObjects.erase(trigger);
                    }
                }
            }
        }
    }

    void onTriggerEnter(TiledObject *trigger)
    {
        std::cout << "[TriggerSystem] Trigger activated!\n";
        std::cout << "  Destination: " << trigger->getProperty("destination") << "\n";
        std::cout << "  Target: " << trigger->getProperty("target") << "\n";

        if (onTeleportCallback) {
            onTeleportCallback( trigger->getProperty("destination"), trigger->getProperty("target"));
        } else {
            std::cerr << "[TriggerSystem] WARNING: No teleport callback set!\n";
        }
    }
};