//
// Created by expleoene on 3/27/26.
//

#ifndef VIEWER_COMPONENT_H
#define VIEWER_COMPONENT_H


namespace BiBuild {
    class SceneObject;

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