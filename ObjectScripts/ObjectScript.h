//
// Created by expleoene on 4/20/26.
//

#ifndef VIEWER_OBJECTSCRIPT_H
#define VIEWER_OBJECTSCRIPT_H

namespace BiBuild {
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
