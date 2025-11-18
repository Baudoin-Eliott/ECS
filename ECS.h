#pragma once

/*
 * ============================================================================
 * ECS - Entity Component System
 * ============================================================================
 * Un système ECS générique et performant pour vos projets de jeux 2D
 *
 * Architecture:
 * - Entity: Simple ID qui contient des composants
 * - Component: Données pures (struct/class)
 * - System: Logique qui opère sur les entités ayant certains composants
 *
 * Usage:
 *   #include "ECS.h"
 *
 *   ECS::Manager manager;
 *   auto& entity = manager.createEntity();
 *   entity.addComponent<TransformComponent>(0, 0);
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <set>

namespace ECS {

    // ========================================================================
    // FORWARD DECLARATIONS
    // ========================================================================
    class Component;
    class Entity;
    class System;
    class Manager;

    // ========================================================================
    // TYPE DEFINITIONS
    // ========================================================================

    // ID unique pour chaque type de composant
    using ComponentID = std::size_t;

    // ID unique pour chaque entité
    using EntityID = std::size_t;

    // Nombre maximum de composants différents (peut être augmenté si nécessaire)
    constexpr std::size_t MAX_COMPONENTS = 64;

    // Bitset pour savoir quels composants une entité possède (rapide et efficace)
    using ComponentBitSet = std::bitset<MAX_COMPONENTS>;

    // Array pour accéder rapidement aux composants par leur ID
    using ComponentArray = std::array<Component*, MAX_COMPONENTS>;

    // ========================================================================
    // LAYER SYSTEM
    // ========================================================================

    /*
     * Les Layers permettent de grouper les entités pour un filtrage rapide
     * Une entité peut appartenir à plusieurs layers simultanément
     *
     * Exemple d'utilisation:
     *   enum class GameLayer {
     *       Default = 0,
     *       Player = 1,
     *       Enemy = 2,
     *       Projectile = 3,
     *       UI = 4
     *   };
     */
    using Layer = std::size_t;
    constexpr std::size_t MAX_LAYERS = 32;
    using LayerBitSet = std::bitset<MAX_LAYERS>;

    // ========================================================================
    // COMPONENT ID GENERATOR
    // ========================================================================

    /*
     * Génère un ID unique pour chaque TYPE de composant
     * Utilise std::type_index pour garantir l'unicité même entre différentes
     * unités de compilation (résout le problème des static dans les templates)
     */
    namespace Internal {
        inline ComponentID getUniqueComponentID() {
            static ComponentID lastID = 0;
            return lastID++;
        }

        // Map globale: type -> ID (thread-safe en C++11+)
        inline std::unordered_map<std::type_index, ComponentID>& getComponentTypeMap() {
            static std::unordered_map<std::type_index, ComponentID> typeMap;
            return typeMap;
        }
    }

    /*
     * Récupère l'ID d'un type de composant
     * Crée un nouvel ID si c'est la première fois qu'on voit ce type
     */
    template <typename T>
    inline ComponentID getComponentTypeID() {
        std::type_index typeIdx(typeid(T));
        auto& typeMap = Internal::getComponentTypeMap();

        // Si ce type n'a pas encore d'ID, on lui en attribue un
        if (typeMap.find(typeIdx) == typeMap.end()) {
            typeMap[typeIdx] = Internal::getUniqueComponentID();
        }

        return typeMap[typeIdx];
    }

    // ========================================================================
    // COMPONENT BASE CLASS
    // ========================================================================

    /*
     * Classe de base pour tous les composants
     * Les composants sont de simples données avec quelques hooks optionnels
     */
    class Component {
    public:
        Entity* entity = nullptr;  // Référence vers l'entité propriétaire

        // Hooks du cycle de vie (optionnels, override si nécessaire)
        virtual void init() {}      // Appelé après ajout à l'entité
        virtual void update() {}    // Appelé chaque frame (si vous n'utilisez pas de Systems)
        virtual void draw() {}      // Appelé pour le rendu (si vous n'utilisez pas de Systems)

        virtual ~Component() = default;
    };

    // ========================================================================
    // ENTITY CLASS
    // ========================================================================

    /*
     * Une entité est simplement un conteneur de composants avec un ID unique
     * Elle ne contient pas de logique, juste des données (composants)
     */
    class Entity {
    private:
        Manager* manager = nullptr;
        EntityID id = 0;
        bool active = true;
        std::string tag = "";
        LayerBitSet layers;

        // Stockage des composants
        std::vector<std::unique_ptr<Component>> components;
        ComponentArray componentArray{};
        ComponentBitSet componentBitSet;

        friend class Manager;
        friend class System;

    public:
        Entity(Manager* mgr, EntityID entityID) : manager(mgr), id(entityID) {}

        // ====================================================================
        // LIFECYCLE
        // ====================================================================

        void update() {
            for (auto& c : components) {
                c->update();
            }
        }

        void draw() {
            for (auto& c : components) {
                c->draw();
            }
        }

        bool isActive() const { return active; }
        void destroy() { active = false; }
        EntityID getID() const { return id; }

        // ====================================================================
        // TAG SYSTEM
        // ====================================================================

        /*
         * Un tag est un identifiant unique (string) pour cette entité
         * Utile pour retrouver rapidement des entités spécifiques (Player, Boss, etc.)
         */
        void setTag(const std::string& t) { tag = t; }
        const std::string& getTag() const { return tag; }
        bool hasTag(const std::string& t) const { return tag == t; }

        // ====================================================================
        // LAYER SYSTEM
        // ====================================================================

        /*
         * Les layers permettent de grouper les entités (une entité peut avoir plusieurs layers)
         * Exemple: une entité peut être à la fois Enemy + Renderable + Collidable
         */
        void addLayer(Layer layer) {
            if (layer < MAX_LAYERS) {
                layers.set(layer);
            }
        }

        void removeLayer(Layer layer) {
            if (layer < MAX_LAYERS) {
                layers.reset(layer);
            }
        }

        bool hasLayer(Layer layer) const {
            return layer < MAX_LAYERS && layers.test(layer);
        }

        const LayerBitSet& getLayers() const { return layers; }

        // ====================================================================
        // COMPONENT MANAGEMENT
        // ====================================================================

        /*
         * Vérifie si l'entité possède un composant d'un certain type
         * Exemple: entity.hasComponent<TransformComponent>()
         */
        template<typename T>
        bool hasComponent() const {
            return componentBitSet[getComponentTypeID<T>()];
        }

        /*
         * Ajoute un composant à l'entité
         *
         * Template variadique: permet de passer n'importe quels arguments au constructeur
         * Perfect forwarding: préserve les références et évite les copies inutiles
         *
         * Exemple:
         *   entity.addComponent<TransformComponent>(10.0f, 20.0f);
         *   entity.addComponent<VelocityComponent>();
         */
        template <typename T, typename... TArgs>
        T& addComponent(TArgs&&... args) {
            // Vérification: pas plus de MAX_COMPONENTS types différents
            ComponentID typeID = getComponentTypeID<T>();
            if (typeID >= MAX_COMPONENTS) {
                throw std::runtime_error("MAX_COMPONENTS exceeded!");
            }

            // Création du composant avec perfect forwarding des arguments
            T* component = new T(std::forward<TArgs>(args)...);
            component->entity = this;

            // Stockage dans un unique_ptr pour gestion automatique de la mémoire
            std::unique_ptr<Component> uPtr{ component };
            components.emplace_back(std::move(uPtr));

            // Enregistrement dans l'array d'accès rapide et le bitset
            componentArray[typeID] = component;
            componentBitSet[typeID] = true;

            // Appel du hook d'initialisation
            component->init();

            return *component;
        }

        /*
         * Récupère un composant par son type
         * ATTENTION: crash si le composant n'existe pas! Vérifiez avec hasComponent() avant
         *
         * Exemple:
         *   if (entity.hasComponent<TransformComponent>()) {
         *       auto& transform = entity.getComponent<TransformComponent>();
         *   }
         */
        template<typename T>
        T& getComponent() const {
            auto ptr = componentArray[getComponentTypeID<T>()];
            return *static_cast<T*>(ptr);
        }

        /*
         * Retire un composant de l'entité
         */
        template<typename T>
        void removeComponent() {
            ComponentID typeID = getComponentTypeID<T>();
            if (componentBitSet[typeID]) {
                componentBitSet[typeID] = false;
                componentArray[typeID] = nullptr;

                // Suppression du composant du vecteur
                components.erase(
                    std::remove_if(components.begin(), components.end(),
                        [typeID](const std::unique_ptr<Component>& c) {
                            return dynamic_cast<T*>(c.get()) != nullptr;
                        }),
                    components.end()
                );
            }
        }
    };

    // ========================================================================
    // SYSTEM BASE CLASS
    // ========================================================================

    /*
     * Classe de base pour tous les systèmes
     * Un système contient la logique qui opère sur les entités ayant certains composants
     *
     * Exemple d'utilisation:
     *   class MovementSystem : public ECS::System {
     *   public:
     *       MovementSystem() {
     *           requireComponent<TransformComponent>();
     *           requireComponent<VelocityComponent>();
     *       }
     *
     *       void update(float deltaTime) override {
     *           for (auto entity : getEntities()) {
     *               auto& transform = entity->getComponent<TransformComponent>();
     *               auto& velocity = entity->getComponent<VelocityComponent>();
     *               transform.x += velocity.x * deltaTime;
     *           }
     *       }
     *   };
     */
    class System {
    protected:
        Manager* manager = nullptr;
        ComponentBitSet componentSignature;  // Quels composants ce système requiert
        std::vector<Entity*> entities;        // Entités qui matchent la signature
        int priority = 0;                     // Ordre d'exécution (plus petit = exécuté en premier)

    public:
        virtual ~System() = default;

        /*
         * Déclare qu'un composant est requis pour ce système
         * À appeler dans le constructeur de votre système
         */
        template<typename T>
        void requireComponent() {
            ComponentID typeID = getComponentTypeID<T>();
            componentSignature.set(typeID);
        }

        /*
         * Vérifie si une entité correspond à la signature de ce système
         */
        bool matchesSignature(const Entity& entity) const {
            return (componentSignature & entity.componentBitSet) == componentSignature;
        }

        const std::vector<Entity*>& getEntities() const { return entities; }

        void setPriority(int p) { priority = p; }
        int getPriority() const { return priority; }

        // Hooks du cycle de vie
        virtual void init() {}                        // Appelé à l'ajout du système
        virtual void update(float deltaTime) {}       // Appelé chaque frame
        virtual void onEntityAdded(Entity* entity) {} // Appelé quand une entité matche
        virtual void onEntityRemoved(Entity* entity) {} // Appelé quand une entité ne matche plus

        friend class Manager;
    };

    // ========================================================================
    // MANAGER CLASS
    // ========================================================================

    /*
     * Le Manager gère toutes les entités et systèmes
     * C'est le point d'entrée principal de l'ECS
     */
    class Manager {
    private:
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<System>> systems;
        std::unordered_map<std::string, Entity*> taggedEntities;
        EntityID nextEntityID = 0;

    public:
        // ====================================================================
        // ENTITY MANAGEMENT
        // ====================================================================

        /*
         * Crée une nouvelle entité
         * Retourne une référence vers l'entité créée
         */
        Entity& createEntity() {
            auto entity = std::make_unique<Entity>(this, nextEntityID++);
            Entity* entityPtr = entity.get();
            entities.emplace_back(std::move(entity));
            return *entityPtr;
        }

        /*
         * Crée une entité avec un tag
         */
        Entity& createEntity(const std::string& tag) {
            Entity& entity = createEntity();
            entity.setTag(tag);
            taggedEntities[tag] = &entity;
            return entity;
        }

        /*
         * Récupère toutes les entités
         */
        const std::vector<std::unique_ptr<Entity>>& getEntities() const {
            return entities;
        }

        /*
         * Récupère une entité par son tag
         * Retourne nullptr si aucune entité n'a ce tag
         */
        Entity* getEntityByTag(const std::string& tag) {
            auto it = taggedEntities.find(tag);
            return (it != taggedEntities.end()) ? it->second : nullptr;
        }

        /*
         * Récupère toutes les entités d'un layer spécifique
         */
        std::vector<Entity*> getEntitiesByLayer(Layer layer) {
            std::vector<Entity*> result;
            for (auto& entity : entities) {
                if (entity->isActive() && entity->hasLayer(layer)) {
                    result.push_back(entity.get());
                }
            }
            return result;
        }

        /*
         * Supprime les entités marquées comme inactives
         * À appeler à la fin de chaque frame
         */
        void refresh() {
            // Mise à jour des systèmes avant suppression
            for (auto& system : systems) {
                system->entities.erase(
                    std::remove_if(system->entities.begin(), system->entities.end(),
                        [&](Entity* entity) {
                            bool shouldRemove = !entity->isActive() || !system->matchesSignature(*entity);
                            if (shouldRemove) {
                                system->onEntityRemoved(entity);
                            }
                            return shouldRemove;
                        }),
                    system->entities.end()
                );
            }

            // Suppression des entités inactives
            entities.erase(
                std::remove_if(entities.begin(), entities.end(),
                    [&](const std::unique_ptr<Entity>& entity) {
                        if (!entity->isActive()) {
                            // Retrait du map des tags
                            if (!entity->getTag().empty()) {
                                taggedEntities.erase(entity->getTag());
                            }
                            return true;
                        }
                        return false;
                    }),
                entities.end()
            );
        }

        // ====================================================================
        // SYSTEM MANAGEMENT
        // ====================================================================

        /*
         * Ajoute un système au manager
         * Les systèmes sont automatiquement triés par priorité
         *
         * Exemple:
         *   manager.addSystem<MovementSystem>();
         *   manager.addSystem<RenderSystem>()->setPriority(100);
         */
        template<typename T, typename... TArgs>
        T* addSystem(TArgs&&... args) {
            T* system = new T(std::forward<TArgs>(args)...);
            system->manager = this;

            std::unique_ptr<System> uPtr{ system };
            systems.emplace_back(std::move(uPtr));

            // Tri par priorité (plus petit en premier)
            std::sort(systems.begin(), systems.end(),
                [](const std::unique_ptr<System>& a, const std::unique_ptr<System>& b) {
                    return a->getPriority() < b->getPriority();
                });

            system->init();
            return system;
        }

        /*
         * Met à jour tous les systèmes
         * À appeler chaque frame
         */
        void update(float deltaTime) {
            // Mise à jour des entités dans les systèmes
            updateSystemEntities();

            // Mise à jour de tous les systèmes
            for (auto& system : systems) {
                system->update(deltaTime);
            }
        }

        /*
         * Met à jour les entités de chaque système selon leur signature
         */
        void updateSystemEntities() {
            for (auto& system : systems) {
                for (auto& entity : entities) {
                    if (entity->isActive() && system->matchesSignature(*entity)) {
                        // Vérifier si l'entité n'est pas déjà dans le système
                        auto it = std::find(system->entities.begin(), system->entities.end(), entity.get());
                        if (it == system->entities.end()) {
                            system->entities.push_back(entity.get());
                            system->onEntityAdded(entity.get());
                        }
                    }
                }
            }
        }

        /*
         * Récupère un système par son type
         */
        template<typename T>
        T* getSystem() {
            for (auto& system : systems) {
                T* castedSystem = dynamic_cast<T*>(system.get());
                if (castedSystem) {
                    return castedSystem;
                }
            }
            return nullptr;
        }
    };

} // namespace ECS