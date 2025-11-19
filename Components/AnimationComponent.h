#pragma once
#include "../ECS.h"
#include <SDL2/SDL.h>
#include <map>
#include <string>

/*
 * ============================================================================
 * AnimationComponent - Gestion des animations avec spritesheets
 * ============================================================================
 * G�re les animations d'une entit� � partir d'un spritesheet
 *
 * D�pend de: SpriteComponent (pour changer le srcRect)
 * Utilis� par: AnimationSystem
 *
 * Concept:
 * - Une animation = une ligne dans le spritesheet + nombre de frames + vitesse
 * - On peut avoir plusieurs animations (Idle, Walk, Attack, etc.)
 * - On switch entre les animations avec Play()
 *
 * Usage:
 *   std::map<std::string, Animation> anims = {
 *       {"Idle", {0, 4, 100}},   // ligne 0, 4 frames, 100ms
 *       {"Walk", {1, 8, 80}}     // ligne 1, 8 frames, 80ms
 *   };
 *   entity.addComponent<AnimationComponent>("Idle", anims);
 *
 *   // Plus tard, pour changer d'animation:
 *   auto& anim = entity.getComponent<AnimationComponent>();
 *   anim.Play("Walk");
 * ============================================================================
 */

 /*
  * Structure repr�sentant une animation
  */
struct Animation {
    int index;      // Ligne (y) dans le spritesheet (0 = premi�re ligne)
    int frames;     // Nombre total de frames dans cette animation
    int speed;      // Vitesse en millisecondes par frame (100 = 10 FPS)
    bool loop;      // Si true, l'animation boucle, sinon elle s'arr�te � la fin

    Animation() : index(0), frames(1), speed(100), loop(true) {}

    Animation(int idx, int frms, int spd, bool lp = true)
        : index(idx), frames(frms), speed(spd), loop(lp) {}
};

class AnimationComponent : public ECS::Component {
public:
    // �tat actuel de l'animation (nom de l'animation en cours)
    std::string currentAnimState;

    // Frame actuellement affich�e dans la boucle (0 � frames-1)
    int currentFrame = 0;

    // Temps du dernier changement de frame (en ms depuis le d�marrage)
    Uint64 lastFrameTime = 0;

    // Collection de toutes les animations disponibles pour cette entit�
    std::map<std::string, Animation> animations;

    // Si false, l'animation ne se met pas � jour
    bool isPlaying = true;

    // ========================================================================
    // CONSTRUCTEURS
    // ========================================================================

    /*
     * Constructeur par d�faut
     */
    AnimationComponent() = default;

    /*
     * Constructeur avec �tat initial et map d'animations
     *
     * Param�tres:
     * - defaultState: Nom de l'animation � jouer au d�part
     * - anims: Map des animations disponibles
     *
     * Exemple:
     *   std::map<std::string, Animation> playerAnims = {
     *       {"Idle", {0, 4, 150}},
     *       {"Walk", {1, 6, 100}},
     *       {"Jump", {2, 3, 80, false}}  // Ne boucle pas
     *   };
     *   entity.addComponent<AnimationComponent>("Idle", playerAnims);
     */
    AnimationComponent(const std::string& defaultState,
        const std::map<std::string, Animation>& anims)
        : currentAnimState(defaultState), animations(anims) {
        lastFrameTime = SDL_GetTicks();
    }

    // ========================================================================
    // M�THODES
    // ========================================================================

    /*
     * Change l'animation en cours
     * Si l'animation demand�e est d�j� en cours, ne fait rien
     * R�initialise la frame � 0 lors du changement
     */
    void Play(const std::string& animName) {
        // V�rifier que l'animation existe
        if (animations.find(animName) == animations.end()) {
            std::cerr << "[AnimationComponent] Animation '" << animName
                << "' not found!\n";
            return;
        }

        // Si on est d�j� sur cette animation, ne rien faire
        if (currentAnimState == animName) {
            return;
        }

        // Changer d'animation
        currentAnimState = animName;
        currentFrame = 0;
        lastFrameTime = SDL_GetTicks();
        isPlaying = true;
    }

    /*
     * Arr�te l'animation (freeze la frame actuelle)
     */
    void Stop() {
        isPlaying = false;
    }

    /*
     * Reprend l'animation
     */
    void Resume() {
        isPlaying = true;
        lastFrameTime = SDL_GetTicks();
    }

    /*
     * Remet l'animation actuelle � la frame 0
     */
    void Reset() {
        currentFrame = 0;
        lastFrameTime = SDL_GetTicks();
    }

    /*
     * V�rifie si l'animation actuelle est termin�e (pour les anims non-loop)
     */
    bool isFinished() const {
        if (animations.find(currentAnimState) == animations.end()) {
            return true;
        }

        const Animation& anim = animations.at(currentAnimState);
        return !anim.loop && currentFrame >= anim.frames - 1;
    }

    /*
     * R�cup�re l'animation actuelle
     */
    const Animation& getCurrentAnimation() const {
        return animations.at(currentAnimState);
    }

    /*
     * Ajoute une nouvelle animation � la map
     */
    void addAnimation(const std::string& name, const Animation& anim) {
        animations[name] = anim;
    }

    void init() override {
        lastFrameTime = SDL_GetTicks();
    }
};