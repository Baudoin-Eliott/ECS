#pragma once
#include "../ECS.h"

// Forward declarations
class SpriteComponent;
class AnimationComponent;
struct Animation;

class AnimationSystem : public ECS::System {
private:
    int tileWidth; 
    int tileHeight;  

public:

    AnimationSystem(int tileW, int tileH);

    void update(float deltaTime) override;

private:

    void updateSpriteRect(SpriteComponent& sprite,
        const AnimationComponent& anim,
        const Animation& currentAnim);
};