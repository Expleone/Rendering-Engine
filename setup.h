//
// Created by expleoene on 5/6/26.
//

#ifndef VIEWER_SETUP_H
#define VIEWER_SETUP_H
#include <string>
#include <vector>

#include "components/ModelComponent.h"
#include "core/Curve.h"
#include "core/InputHandler.h"
#include "core/RenderSystem.h"
#include "core/ResourceManager.h"
#include "ObjectScripts/BiplaneScript.h"
#include "ObjectScripts/TVScript.h"
#include "ObjectScripts/CameraScript.h"



extern float waveHeight;
extern float waveSpeed;

extern float windStrength;
extern float windSpeed;
extern glm::vec3 windDir;

extern float tv_minX;
extern float tv_minY;
extern float tv_maxX;
extern float tv_maxY;

extern float dayLengthSeconds;

extern char* font;

extern std::vector<std::string> skyboxFaces;
extern std::vector<std::string> nightSkyboxFaces;

extern std::vector<std::pair<std::string, holubiho::KeyBind>> stdActions;

extern void loadConfig(const std::string& filepath);

extern std::vector<std::pair<std::string, holubiho::KeyBind>> LoadKeyBindsTXT(const std::string& filepath);
extern void SetupInputBindings();

extern holubiho::SceneObject* SetupTv(holubiho::SceneManager* scene );
extern void LoadTVVideosTXT(const std::string& filepath, holubiho::SceneObject* tv);

extern holubiho::SceneObject* SetupPlane(holubiho::SceneManager* scene, bool* debugPtr);

extern holubiho::SceneObject* SetupCamera(holubiho::SceneManager* scene, bool* debugPtr);



#endif //VIEWER_SETUP_H
