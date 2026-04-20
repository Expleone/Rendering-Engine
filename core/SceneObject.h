//
// Created by expleone on 3/27/26.
//

#ifndef VIEWER_SCENEOBJECT_H
#define VIEWER_SCENEOBJECT_H
#include <uuid.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../components/Component.h"
#include "../components/TransformComponent.h"

namespace BiBuild {


    class SceneObject {
    public:
        uuids::uuid uuid;
        std::string name;
        TransformComponent* transform;

        std::vector<std::unique_ptr<Component>> components;
        // std::vector<std::unique_ptr<Component>>

        SceneObject(uuids::uuid id, std::string n) : uuid(id), name(std::move(n)), transform(nullptr) {
            transform = AddComponent<TransformComponent>();
        }

        template <typename T>
        T* AddComponent();

        template <typename T>
        T* GetComponent() const;

        bool AddChild(SceneObject* child);
    };

    template <typename T>
    T* SceneObject::AddComponent() {
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
} // BiBuild

#endif //VIEWER_SCENEOBJECT_H