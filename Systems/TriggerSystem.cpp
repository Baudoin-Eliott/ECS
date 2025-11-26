#include "TriggerSystem.h"
#include "../Components/CollisionComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TileMapComponent.h"
#include "../../src/Components/PlayerComponent.h"
#include "../../src/Managers/AudioManager.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

TriggerSystem::TriggerSystem()

    {
        requireComponent<TransformComponent>();
        requireComponent<CollisionComponent>();
    }

    void TriggerSystem::setTeleportCallback(std::function<void(const std::string &, const std::string &)> callback)
    
    {
        onTeleportCallback = callback;
    }

    void TriggerSystem::update(float deltaTime)
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

    void TriggerSystem::onTriggerEnter(TiledObject *trigger)
    {
        std::cout << "[TriggerSystem] Trigger activated!\n";
        std::cout << "  Destination: " << trigger->getProperty("destination") << "\n";
        std::cout << "  Target: " << trigger->getProperty("target") << "\n";

        if (onTeleportCallback)
        {
            AudioManager::getInstance().playSound("teleport");

            AudioManager::getInstance().stopMusic(1000);
            onTeleportCallback(trigger->getProperty("destination"), trigger->getProperty("target"));
        }
        else
        {
            std::cerr << "[TriggerSystem] WARNING: No teleport callback set!\n";
        }
    }
