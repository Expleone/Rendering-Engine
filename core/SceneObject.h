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
#include "../components/Component.h"
#include "../components/TransformComponent.h"
#include "../ObjectScripts/ObjectScript.h"

namespace BiBuild {


    class SceneObject {
    public:
        uuids::uuid uuid;
        std::string name;
        TransformComponent* transform;

        std::vector<std::unique_ptr<Component>> components;
        std::vector<std::unique_ptr<ObjectScript>> scripts;

        SceneObject(uuids::uuid id, std::string n) : uuid(id), name(std::move(n)), transform(nullptr) {
            transform = AddComponent<TransformComponent>();
        }

        template <typename T>
        T* AddComponent();

        template <typename T>
        T* GetComponent() const;


        template <typename T>
        T* AddScript();

        void ExecuteScripts();

        bool AddChild(SceneObject* child);
    };

    template <typename T>
    T* SceneObject::AddComponent() {
        if (!std::is_base_of_v<Component, T> ) return nullptr;
        auto comp = std::make_unique<T>(this);
        T* ptr = comp.get();
        components.push_back(std::move(comp));
        return ptr;
    }

    template <typename T>
    T* SceneObject::GetComponent() const {
        for (const auto& comp : components) {
            if (T* casted = dynamic_cast<T*>(comp.get())) {
                return casted;
            }
        }
        return nullptr;
    }

    template <typename T>
    T* SceneObject::AddScript() {
        if (!std::is_base_of_v<ObjectScript, T> ) return nullptr;
        auto script = std::make_unique<T>(this);
        auto ptr = script.get();
        scripts.push_back(std::move(script));
        return ptr;
    }

    inline void SceneObject::ExecuteScripts() {
        int size = scripts.size();
        // std::cout << "There are " + std::to_string(size) + " scripts";
        for (auto& script : scripts) {

            script->Update();
        }
    }
} // BiBuild

#endif //VIEWER_SCENEOBJECT_H