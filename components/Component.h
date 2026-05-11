//
// Created by expleoene on 3/27/26.
//

#ifndef VIEWER_COMPONENT_H
#define VIEWER_COMPONENT_H


namespace holubiho {
    class SceneObject;

    /// @brief Base class for all components that can be attached to SceneObjects. Provides a pointer to the owning SceneObject and a virtual destructor for proper cleanup of derived components.
    class Component {
    protected:
        SceneObject* owner = nullptr;
    public:
        Component(SceneObject* owner) : owner(owner){}
        virtual ~Component() = default;
        [[nodiscard]] SceneObject* GetOwner() const { return owner; }
    };
} // BiBuild

#endif //VIEWER_COMPONENT_H