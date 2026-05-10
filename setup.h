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
#include "test_models/cube.h"
#include "ObjectScripts/CameraScript.h"



static float waveHeight = 0.14f;
static float waveSpeed = 1.0f;


static float windStrength = 0.05f;
static float windSpeed = 1.0f;
static glm::vec3 windDir = glm::vec3(1,0,1);

// TV screen UV bounds (must be persistent so we can pass their addresses to materials)
static float tv_minX = 0.008f;
static float tv_minY = 0.786f;
static float tv_maxX = 0.264f;
static float tv_maxY = 0.997f;

std::vector<std::string> skyboxFaces
{
    "resources/textures/skyboxes/Textures/SkyNoon_Right.png",
    "resources/textures/skyboxes/Textures/SkyNoon_Left.png",
    "resources/textures/skyboxes/Textures/SkyNoon_Top.png",
    "resources/textures/skyboxes/Textures/SkyNoon_Bottom.png",
    "resources/textures/skyboxes/Textures/SkyNoon_Front.png",
    "resources/textures/skyboxes/Textures/SkyNoon_Back.png",
};
std::vector<std::string> nightSkyboxFaces
{
    "resources/textures/skyboxes/Textures/SkyNight_Right.png",
    "resources/textures/skyboxes/Textures/SkyNight_Left.png",
    "resources/textures/skyboxes/Textures/SkyNight_Top.png",
    "resources/textures/skyboxes/Textures/SkyNight_Bottom.png",
    "resources/textures/skyboxes/Textures/SkyNight_Front.png",
    "resources/textures/skyboxes/Textures/SkyNight_Back.png",
};

std::vector<std::pair<std::string, BiBuild::KeyBind>> stdActions{
    {"Interact", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
        {BiBuild::InputDevice::MOUSE, GLFW_MOUSE_BUTTON_LEFT}
    }}},
    {"MoveCamera", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
        {BiBuild::InputDevice::MOUSE, GLFW_MOUSE_BUTTON_RIGHT}
    }}},
    {"MoveForward", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                    {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_W}
    }}},
    {"MoveBackward", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                        {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_S}
    }}},
    {"MoveRight", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                            {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_D}
    }}},
    {"MoveLeft", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                            {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_A}
    }}},
    {"MoveUp", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_Q}
    }}},
    {"MoveDown", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_E}
    }}},
    {"Sprint", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                            {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_LEFT_SHIFT}
    }}},
    {"MoveToNextPoint", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                    {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_M},
                                    {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_N}
    }}},
    {"StopMovingAlongCurve", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                        {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_P}
    }}},
    {"ToggleDebug", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                        {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_L}
    }}},
    {"ToggleUUID", BiBuild::KeyBind{std::vector<BiBuild::InputToken>{
                                                {BiBuild::InputDevice::KEYBOARD, GLFW_KEY_U}
    }}},
};


std::vector<std::pair<std::string, BiBuild::KeyBind>> LoadKeyBindsTXT(const std::string& filepath) {
    std::vector<std::pair<std::string, BiBuild::KeyBind>> actions;
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << "\n";
        return actions;
    }

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string actionName;
        ss >> actionName;

        std::vector<BiBuild::InputToken> tokens;
        std::string deviceStr;
        int keyCode;

        while (ss >> deviceStr >> keyCode) {
            BiBuild::InputDevice device = (deviceStr == "MOUSE")
                                        ? BiBuild::InputDevice::MOUSE
                                        : BiBuild::InputDevice::KEYBOARD;

            tokens.push_back({device, keyCode});
        }

        if (!tokens.empty()) {
            actions.push_back({actionName, BiBuild::KeyBind{tokens}});
        }
    }

    return actions;
}

void LoadTVVideosTXT(const std::string& filepath, BiBuild::SceneObject* tv) {
    auto tvScript = tv->GetScript<BiBuild::TVScript>();
     std::vector<std::string> videos;
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << "\n";
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        videos.push_back(line);
    }

    tvScript->SetContent(videos);
}

void SetupInputBindings() {
    auto actions = LoadKeyBindsTXT("keybinds.txt");
    for (auto& action : actions) {
        BiBuild::InputManager::RegisterAction(action.first, action.second);
    }
}

BiBuild::SceneObject* SetupTv(BiBuild::SceneManager* scene ) {
    auto film = scene->CreateObject("Film");
    auto filmScript = film->AddScript<BiBuild::FilmScript>();
    
    auto tv = scene->CreateObject("tv");

    // tmod->mesh = BiBuild::ResourceManager::GetMesh("./resources/tv/TV01.obj");
    auto meshes = BiBuild::ResourceManager::LoadMeshesFromFile("./resources/tv/TV.obj");
    for (auto mesh : meshes) {
        auto model = tv->AddComponent<BiBuild::ModelComponent>();
        model->mesh = mesh;
    }
    auto tmod = tv->GetAllComponents<BiBuild::ModelComponent>();
    tmod[0]->drawUUID = true;
    tmod[0]->mat = BiBuild::ResourceManager::CreateMaterial("TV_Screen");
    tmod[0]->mat->textures.push_back(BiBuild::ResourceManager::GetTexture("resources/tv/uploads_files_6850290_RetroTV+Textures/4k/Television_BaseColor.png"));
    tmod[0]->mat->textures.push_back(BiBuild::ResourceManager::GetTexture("resources/tv/uploads_files_6850290_RetroTV+Textures/4k/Television_Normal.png"));
    tmod[0]->mat->textures.push_back(filmScript->GetTexture());
    tmod[0]->mat->shader = BiBuild::ResourceManager::LoadShaderProgram("TV_shader", "./shaders/vertex/base.vert", "./shaders/fragment/tv_screen.frag");
    // Use persistent TV UV bounds so the material keeps valid pointers
    tmod[0]->mat->AddInfo("minX", &tv_minX, BiBuild::UniformType::Float);
    tmod[0]->mat->AddInfo("maxX", &tv_maxX, BiBuild::UniformType::Float);
    tmod[0]->mat->AddInfo("minY", &tv_minY, BiBuild::UniformType::Float);
    tmod[0]->mat->AddInfo("maxY", &tv_maxY, BiBuild::UniformType::Float);
    auto tvscr = tv->AddScript<BiBuild::TVScript>();
    tv->transform->localPosition = glm::vec3(0.270f, 0.720f, -11.142f);
    tv->transform->localRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0));

    
    auto nextButton = scene->CreateObject("NextButton");
    auto nextModel = nextButton->AddComponent<BiBuild::ModelComponent>();
    nextModel->mesh = BiBuild::ResourceManager::GetMesh("cube_mesh");
    nextModel->mat = BiBuild::ResourceManager::CreateMaterial("NextButtonMat");
    nextModel->mat->ambient = glm::vec4(0.0f);
    nextModel->mat->diffuse = glm::vec4(0.0f, 0 , 0 ,1.0f);
    nextModel->mat->specular = glm::vec3(0.0f);
    nextModel->mat->emission = glm::vec3(1.0f, 0.5f, 0.0f);
    nextModel->mat->shininess = 1.0f;
    nextButton->transform->localPosition = glm::vec3(0.293f, 0.09f, 0.49f);
    nextButton->transform->localScale = glm::vec3(0.016f);
    nextButton->hasClickableParts = true;
    nextModel->isClickable = true;
    tv->AddChild(nextButton);


    auto playButton = scene->CreateObject("PlayButton");
    auto playModel = playButton->AddComponent<BiBuild::ModelComponent>();
    playModel->mesh = BiBuild::ResourceManager::LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));
    playModel->mat = BiBuild::ResourceManager::CreateMaterial("PlayButtonMat");
    playModel->mat->ambient = glm::vec4(0.0f);
    playModel->mat->diffuse = glm::vec4(0.0f, 0 , 0 ,1.0f);
    playModel->mat->specular = glm::vec3(0.0f);
    playModel->mat->emission = glm::vec3(0.0f, 1.0f, 0.0f);
    playModel->mat->shininess = 1.0f;
    playButton->transform->localPosition = glm::vec3(0.253f, 0.09f, 0.49f);
    playButton->transform->localScale = glm::vec3(0.016f);
    playModel->isClickable = true;
    playButton->hasClickableParts = true;
    tv->AddChild(playButton);



    auto prevButton = scene->CreateObject("PrevButton");
    auto prevModel = prevButton->AddComponent<BiBuild::ModelComponent>();
    prevModel->mesh = BiBuild::ResourceManager::LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));;
    prevModel->mat = BiBuild::ResourceManager::CreateMaterial("PrevButtonMat");
    prevModel->mat->ambient = glm::vec4(0.0f);
    prevModel->mat->diffuse = glm::vec4(0.0f, 0 , 0 ,1.0f);
    prevModel->mat->specular = glm::vec3(0.0f);
    prevModel->mat->emission = glm::vec3(1.0f, 0.0f, 0.0f);
    prevModel->mat->shininess = 1.0f;
    prevButton->transform->localPosition = glm::vec3(0.213f, 0.09f, 0.49f);
    prevButton->transform->localScale = glm::vec3(0.016f);
    prevModel->isClickable = true;
    prevButton->hasClickableParts = true;
    tv->AddChild(prevButton);

    tvscr->SetFilmScript(filmScript);
    tvscr->SetButtons( playButton, nextButton, prevButton);
    LoadTVVideosTXT("tv_videos.txt", tv);

    auto table = scene->CreateObject("table");
    BiBuild::ResourceManager::LoadModelsFromFile("resources/low_poly_dining_table.glb", table, false );
    table->transform->localPosition = glm::vec3(0.180f, 0.733f, -11.142f);
    table->transform->localRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1,0,0));
    table->transform->localScale = glm::vec3(0.5f);


    auto SpotlightObj = scene->CreateObject("TVLight");
    auto* SpotlightLight = SpotlightObj->AddComponent<BiBuild::LightComponent>();
    SpotlightLight->type        = BiBuild::LightType::Spotlight;
    SpotlightLight->diffuse     = glm::vec3(1);
    SpotlightLight->specular    = glm::vec3(0, 0, 0);
    SpotlightLight->attenuation = glm::vec3(1.0,0.7,1.8);
    SpotlightLight->cutoff      = 90.0f;
    SpotlightLight->direction = -tv->transform->Forward();
    SpotlightObj->transform->localPosition = glm::vec3(-0.07f, 0.2f, 0.428f);
    tvscr->SetLight(SpotlightObj);
    tv->AddChild(SpotlightObj);

    return tv;
}

BiBuild::SceneObject* SetupPlane(BiBuild::SceneManager* scene, bool* debugPtr) {
    auto planeWrapper = scene->CreateObject("planeWrapper");
    auto plane = scene->CreateObject("plane");
    BiBuild::ResourceManager::LoadModelsFromFile("resources/low-poly-biplane/biplane.glb", plane, true);
    auto children = plane->GetChildren();
    plane->transform->localScale = glm::vec3(0.2f);
    glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0,1,0))*glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,0,1));
    plane->transform->Rotate(rot);
    planeWrapper->AddChild(plane);
    auto planeScr = planeWrapper->AddScript<BiBuild::BiplaneScript>();
    planeScr->SetPlane(children[0]);
    children[0]->hasClickableParts = true;
    planeScr->SetAirscrew(children[1]);
    planeWrapper->transform->localPosition = glm::vec3(-5.076f, 1.269f, -26.196f);
    planeWrapper->transform->localRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0,1,0));

    std::vector<glm::vec3> planeControlPoints = {
        glm::vec3(0.0f, 20.0f, 10.0f),   // Start point
        glm::vec3(10.0f, 20.0f, 20.0f),
        glm::vec3(15.0f, 30.0f, 0.0f),
        glm::vec3(20.0f, 40.0f, -20.0f),
        glm::vec3(0.0f, 30.0f, -20.0f),
        glm::vec3(-20.0f, 20.0f, -10.0f),
        glm::vec3(-10.0f, 20.0f, 0.0f),
        glm::vec3(-20.0f, 20.0f, 10.0f)  // End point matches start point to close the curve
    };

    std::vector<BiBuild::SceneObject*> controlPointObjects;
    int cpIndex = 0;
    for (auto point : planeControlPoints) {
        auto cpObj = scene->CreateObject("PlaneControlPoint" + std::to_string(cpIndex++));
        auto cpModel = cpObj->AddComponent<BiBuild::ModelComponent>();
        cpModel->drawUUID = true;
        cpModel->mesh = BiBuild::ResourceManager::LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));
        auto cpMat = BiBuild::ResourceManager::CreateMaterial("controlPointMat");
        cpMat->ambient   = glm::vec4(0.0f); // No self-illumination
        cpMat->diffuse   = glm::vec4(glm::vec3(1, 0, 0), 1); // Red color for control points
        cpMat->specular  = glm::vec3(0.0f); // No specular highlights
        cpMat->emission  = glm::vec3(1, 0, 0); // Make them glow red for visibility
        cpModel->mat = cpMat;
        cpObj->transform->localPosition = point;
        cpObj->transform->localScale = glm::vec3(0.5f);
        controlPointObjects.push_back(cpObj);
    }

    // Instantiate the curve
    BiBuild::Curve closedCurve = BiBuild::Curve::createClosedLoop(planeControlPoints);

    planeScr->SetFlightPath(closedCurve);
    planeScr->SetControlPointObjects(controlPointObjects);
    planeScr->SetDebugPointer(debugPtr);


    return planeWrapper;
}

BiBuild::SceneObject* SetupCamera(BiBuild::SceneManager* scene, bool* debugPtr) {
    auto* camera = scene->cameraObject;
    if (!camera) {
        std::cerr << "CameraComponent is missing on cameraObject." << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(BiBuild::RenderSystem::GetGLFWWindow());
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    auto cameraScript = scene->cameraObject->AddScript<BiBuild::CameraScript>();
    scene->cameraObject->transform->localPosition = glm::vec3(0.0f, 00.0f, 10.0f);
    std::vector<glm::vec3> cameraControlPoints = {
        glm::vec3( -6.00f, 25.00f,  -5.00f), // Top front
        glm::vec3( 10.26f, 20.00f,  -8.52f), // Mid right-front
        glm::vec3( 17.00f, 15.00f, -17.00f), // Bottom right
        glm::vec3( 10.26f, 20.00f, -25.48f), // Mid right-back
        glm::vec3( -6.00f, 25.00f, -29.00f), // Top back
        glm::vec3(-22.26f, 20.00f, -25.48f), // Mid left-back
        glm::vec3(-29.00f, 15.00f, -17.00f), // Bottom left
        glm::vec3(-22.26f, 20.00f,  -8.52f)  // Mid left-front
    };
    int cpIndex = 0;
    std::vector<BiBuild::SceneObject*> cameraControlPointObjects;
    for (auto point : cameraControlPoints) {
        auto cpObj = camera->scene->CreateObject("CameraControlPoint" + std::to_string(cpIndex++));
        auto cpModel = cpObj->AddComponent<BiBuild::ModelComponent>();
        cpModel->drawUUID = true;
        cpObj->render = false;
        cpModel->mesh = BiBuild::ResourceManager::LoadMesh("cube_mesh", cube_data.vertices, cube_data.nVertices*3, cube_data.faces, cube_data.nFaces * 3, glm::vec3(0.2f, 0.7f, 0.3f));
        auto cpMat = BiBuild::ResourceManager::CreateMaterial("CameraControlPointMat");
        cpMat->ambient   = glm::vec4(0.0f); // No self-illumination
        cpMat->diffuse   = glm::vec4(glm::vec3(0, 0, 1), 1); // Red color for control points
        cpMat->specular  = glm::vec3(0.0f); // No specular highlights
        cpMat->emission  = glm::vec3(0, 0, 1); // Make them glow red for visibility
        cpModel->mat = cpMat;
        cpObj->transform->localPosition = point;
        cpObj->transform->localScale = glm::vec3(0.5f);
        cameraControlPointObjects.push_back(cpObj);
    }
    cameraScript->SetControlPointObjects(cameraControlPointObjects);
    cameraScript->SetDebugPointer(debugPtr);
    return  camera;
}



#endif //VIEWER_SETUP_H
