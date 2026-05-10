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

namespace BiBuild {
    class SceneManager;

    class SceneObject {
    public:
        uuids::uuid uuid;
        std::string name;
        TransformComponent* transform;
        bool render = true;
        bool hasClickableParts = false;
        bool hasBeenInteracted = false;
        SceneManager* scene = nullptr;

        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<ObjectScript>> scripts;

        SceneObject(uuids::uuid id, std::string n, SceneManager* scene) : uuid(id), name(std::move(n)), transform(nullptr), scene(scene) {
            transform = AddComponent<TransformComponent>();
        }

        template <typename T>
        T* AddComponent() {
            if (!std::is_base_of_v<Component, T> ) return nullptr;
            auto comp = std::make_unique<T>(this);
            T* ptr = comp.get();
            components.push_back(std::move(comp));
            return ptr;
        }

        template <typename T>
        T* GetComponent() const {
            for (const auto& comp : components) {
                if (T* casted = dynamic_cast<T*>(comp.get())) {
                    return casted;
                }
            }
            return nullptr;
        }

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

        template <typename T>
        T* AddScript() {
            if (!std::is_base_of_v<ObjectScript, T> ) return nullptr;
            auto script = std::make_unique<T>(this);
            auto ptr = script.get();
            scripts.push_back(std::move(script));
            return ptr;
        }
        template <typename T>
        T* GetScript() const {
            for (const auto& scr : scripts) {
                if (T* casted = dynamic_cast<T*>(scr.get())) {
                    return casted;
                }
            }
            return nullptr;
        }

        inline void ExecuteScripts() {
            int size = scripts.size();
            for (auto& script : scripts) {
                script->Update();
            }
        }

        bool AddChild(SceneObject* child);
        bool DeleteChild(SceneObject* child);
        std::vector<SceneObject*> GetChildren() const;
    };


} // BiBuild

#endif //VIEWER_SCENEOBJECT_H