//
// Created by expleone on 3/27/26.
//

#ifndef VIEWER_SCENEOBJECT_H
#define VIEWER_SCENEOBJECT_H
#include <iostream>
#include <uuid.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "SceneManager.h"
#include "../components/Component.h"
#include "../components/TransformComponent.h"
#include "../ObjectScripts/ObjectScript.h"

namespace holubiho {
    class SceneManager;

    /// @brief Represents an object in the scene.
    class SceneObject {
    public:
        uuids::uuid uuid;
        std::string name;
        TransformComponent* transform;
        bool render = true;
        bool hasClickableParts = false;

        /// @brief Indicates whether the object has been interacted with (e.g., clicked) at least once.
        bool hasBeenInteracted = false;
        SceneManager* scene = nullptr;

        /// @brief A vector of unique pointers to Component instances attached to this SceneObject.
        std::vector<std::unique_ptr<Component>> components;
        /// @brief A vector of unique pointers to ObjectScript instances attached to this SceneObject. These scripts define custom behavior that can be executed each frame.
        std::vector<std::unique_ptr<ObjectScript>> scripts;

        SceneObject(uuids::uuid id, std::string n, SceneManager* scene) : uuid(id), name(std::move(n)), transform(nullptr), scene(scene) {
            transform = AddComponent<TransformComponent>();
        }

        /// @brief Adds a new component of type T to the SceneObject.
        template <typename T>
        T* AddComponent() {
            if (!std::is_base_of_v<Component, T> ) return nullptr;
            auto comp = std::make_unique<T>(this);
            T* ptr = comp.get();
            components.push_back(std::move(comp));
            return ptr;
        }

        /// @brief Retrieves the first component of type T attached to the SceneObject. If no such component exists, it returns nullptr.
        template <typename T>
        T* GetComponent() const {
            for (const auto& comp : components) {
                if (T* casted = dynamic_cast<T*>(comp.get())) {
                    return casted;
                }
            }
            return nullptr;
        }

        /// @brief Retrieves all components of type T attached to the SceneObject. Returns a vector of pointers to the components. If no such components exist, it returns an empty vector.
        template <typename T>
        std::vector<T*> GetAllComponents() const {
            std::vector<T*> componentList;
            for (const auto& comp : components) {
                if (T* casted = dynamic_cast<T*>(comp.get())) {
                    componentList.push_back(casted);
                }
            }
            return componentList;
        }

        /// @brief Deletes all components of type T attached to the SceneObject. This method removes the components from the internal vector, effectively destroying them.
        template <typename T>
        void DeleteAllComponents() {
            components.erase(
                std::remove_if(components.begin(), components.end(),
                    [](const std::unique_ptr<Component>& comp) {
                        return dynamic_cast<T*>(comp.get()) != nullptr;
                    }),
                components.end()
            );
        }

        /// @brief Adds a new script of type T to the SceneObject. The script must be derived from ObjectScript. Returns a pointer to the newly added script, or nullptr if T is not a valid script type.
        template <typename T>
        T* AddScript() {
            if (!std::is_base_of_v<ObjectScript, T> ) return nullptr;
            auto script = std::make_unique<T>(this);
            auto ptr = script.get();
            scripts.push_back(std::move(script));
            return ptr;
        }

        /// @brief Retrieves the first script of type T attached to the SceneObject. If no such script exists, it returns nullptr.
        template <typename T>
        T* GetScript() const {
            for (const auto& scr : scripts) {
                if (T* casted = dynamic_cast<T*>(scr.get())) {
                    return casted;
                }
            }
            return nullptr;
        }

        /// @brief Executes the Update method of all scripts attached to the SceneObject. This method should be called once per frame to ensure that all script logic is executed and up to date before rendering.
        void ExecuteScripts() {
            for (auto& script : scripts) {
                script->Update();
            }
        }

        /// @brief Adds a child SceneObject to this SceneObject. The child will be added to the transform hierarchy, and its parent will be set to this SceneObject. Returns true if the child was successfully added, or false if the child is null or is the same as this SceneObject.
        bool AddChild(SceneObject* child);
        /// @brief Removes a child SceneObject from this SceneObject. The child will be removed from the transform hierarchy, and its parent will be set to null. Returns true if the child was successfully removed, or false if the child is null or is not a child of this SceneObject.
        bool DeleteChild(SceneObject* child);

        /// @brief Retrieves a vector of pointers to all child SceneObjects of this SceneObject. If this SceneObject has no children, it returns an empty vector.
        std::vector<SceneObject*> GetChildren() const;
    };


} // BiBuild

#endif //VIEWER_SCENEOBJECT_H