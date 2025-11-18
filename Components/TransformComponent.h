#pragma once
#include "../ECS.h"
#include "../Utils/Vector2D.h"

/*
 * ============================================================================
 * TransformComponent - Position, rotation et échelle d'une entité
 * ============================================================================
 * C'est le composant le plus fondamental, presque toutes les entités en ont un
 *
 * Contient:
 * - position: Où est l'entité dans le monde
 * - velocity: Vitesse de déplacement (utilisé par MovementSystem)
 * - scale: Taille de l'entité (1.0 = taille normale)
 * - rotation: Angle de rotation en degrés
 *
 * Usage:
 *   entity.addComponent<TransformComponent>(100.0f, 200.0f); // x, y
 *   auto& transform = entity.getComponent<TransformComponent>();
 *   transform.position.x += 5;
 * ============================================================================
 */

class TransformComponent : public ECS::Component {
public:
    Vector2D position;
    Vector2D velocity;
    float scale;
    float rotation;  // En degrés

    // ========================================================================
    // CONSTRUCTEURS
    // ========================================================================

    /*
     * Constructeur par défaut: entité à l'origine (0,0)
     */
    TransformComponent()
        : position(0, 0), velocity(0, 0), scale(1.0f), rotation(0.0f) {}

    /*
     * Constructeur avec position initiale
     */
    TransformComponent(float x, float y)
        : position(x, y), velocity(0, 0), scale(1.0f), rotation(0.0f) {}

    /*
     * Constructeur avec position et échelle
     */
    TransformComponent(float x, float y, float s)
        : position(x, y), velocity(0, 0), scale(s), rotation(0.0f) {}

    /*
     * Constructeur complet
     */
    TransformComponent(float x, float y, float s, float rot)
        : position(x, y), velocity(0, 0), scale(s), rotation(rot) {}

    // ========================================================================
    // HOOKS
    // ========================================================================

    void init() override {
        // La vélocité est initialisée à zéro par défaut
    }

    // Update et draw sont gérés par les Systems, pas besoin de les implémenter ici
};