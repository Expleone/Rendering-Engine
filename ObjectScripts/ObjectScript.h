//
// Created by expleoene on 4/20/26.
//

#ifndef VIEWER_OBJECTSCRIPT_H
#define VIEWER_OBJECTSCRIPT_H


// Default base class for all scripts that can be attached to scene objects. Provides a common interface and access to the owning object.
namespace holubiho {
    class SceneObject;

    class ObjectScript {
protected:
    SceneObject* owner = nullptr;
public:
    ObjectScript(SceneObject* owner) : owner(owner){}
    virtual ~ObjectScript() = default;
    virtual void Update();
};

} // BiBuild

#endif //VIEWER_OBJECTSCRIPT_H
