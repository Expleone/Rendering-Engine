//
// Created by expleoene on 5/11/26.
//
#include "setup.h"

float waveHeight = 0.14f;
float waveSpeed = 1.0f;

float windStrength = 0.05f;
float windSpeed = 1.0f;
glm::vec3 windDir = glm::vec3(1,0,1);

float tv_minX = 0.008f;
float tv_minY = 0.786f;
float tv_maxX = 0.264f;
float tv_maxY = 0.997f;

float dayLengthSeconds = 10.0f;

char* font = "resources/fonts/border-wall-font/BorderWall-OG55o.otf";

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

std::vector<std::pair<std::string, holubiho::KeyBind>> stdActions{
    {"Interact", holubiho::KeyBind{std::vector<holubiho::InputToken>{
        {holubiho::InputDevice::MOUSE, GLFW_MOUSE_BUTTON_LEFT}
    }}},
    {"MoveCamera", holubiho::KeyBind{std::vector<holubiho::InputToken>{
        {holubiho::InputDevice::MOUSE, GLFW_MOUSE_BUTTON_RIGHT}
    }}},
    {"MoveForward", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                    {holubiho::InputDevice::KEYBOARD, GLFW_KEY_W}
    }}},
    {"MoveBackward", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                        {holubiho::InputDevice::KEYBOARD, GLFW_KEY_S}
    }}},
    {"MoveRight", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                            {holubiho::InputDevice::KEYBOARD, GLFW_KEY_D}
    }}},
    {"MoveLeft", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                            {holubiho::InputDevice::KEYBOARD, GLFW_KEY_A}
    }}},
    {"MoveUp", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                {holubiho::InputDevice::KEYBOARD, GLFW_KEY_Q}
    }}},
    {"MoveDown", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                {holubiho::InputDevice::KEYBOARD, GLFW_KEY_E}
    }}},
    {"Sprint", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                            {holubiho::InputDevice::KEYBOARD, GLFW_KEY_LEFT_SHIFT}
    }}},
    {"MoveToNextPoint", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                    {holubiho::InputDevice::KEYBOARD, GLFW_KEY_M},
                                    {holubiho::InputDevice::KEYBOARD, GLFW_KEY_N}
    }}},
    {"StopMovingAlongCurve", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                        {holubiho::InputDevice::KEYBOARD, GLFW_KEY_P}
    }}},
    {"ToggleDebug", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                        {holubiho::InputDevice::KEYBOARD, GLFW_KEY_L}
    }}},
    {"ToggleUUID", holubiho::KeyBind{std::vector<holubiho::InputToken>{
                                                {holubiho::InputDevice::KEYBOARD, GLFW_KEY_U}
    }}},
};


std::vector<std::pair<std::string, holubiho::KeyBind>> LoadKeyBindsTXT(const std::string& filepath) {
    std::vector<std::pair<std::string, holubiho::KeyBind>> actions;
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << "\n";
        return actions;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string actionName;
        ss >> actionName;

        std::vector<holubiho::InputToken> tokens;
        std::string deviceStr;
        int keyCode;

        while (ss >> deviceStr >> keyCode) {
            holubiho::InputDevice device = (deviceStr == "MOUSE")
                                        ? holubiho::InputDevice::MOUSE
                                        : holubiho::InputDevice::KEYBOARD;

            tokens.push_back({device, keyCode});
        }

        if (!tokens.empty()) {
            actions.push_back({actionName, holubiho::KeyBind{tokens}});
        }
    }

    return actions;
}

void loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filepath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        std::istringstream iss(line);
        std::string key;

        if (iss >> key) {
            if (key == "waveHeight") {
                iss >> waveHeight;
            } else if (key == "waveSpeed") {
                iss >> waveSpeed;
            } else if (key == "windStrength") {
                iss >> windStrength;
            } else if (key == "windSpeed") {
                iss >> windSpeed;
            } else if (key == "windDir") {
                iss >> windDir.x >> windDir.y >> windDir.z;
            } else if (key == "dayLengthSeconds") {
                iss >> dayLengthSeconds;
            }
        }
    }
    std::cout << "Config loaded from " << filepath << std::endl;
    std::cout << "waveHeight: " << waveHeight << "\n";
    std::cout << "waveSpeed: " << waveSpeed << "\n";
    std::cout << "windStrength: " << windStrength << "\n";
    std::cout << "windSpeed: " << windSpeed << "\n";
    std::cout << "windDir: (" << windDir.x << ", " << windDir.y << ", " << windDir.z << ")\n";
    std::cout << "dayLengthSeconds: " << dayLengthSeconds << "\n";
}

void LoadTVVideosTXT(const std::string& filepath, holubiho::SceneObject* tv) {
    auto tvScript = tv->GetScript<holubiho::TVScript>();
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
        holubiho::InputManager::RegisterAction(action.first, action.second);
    }
}

holubiho::SceneObject* SetupTv(holubiho::SceneManager* scene ) {
    auto film = scene->CreateObject("Film");
    auto filmScript = film->AddScript<holubiho::FilmScript>();

    auto tv = scene->CreateObject("tv");

    // tmod->mesh = BiBuild::ResourceManager::GetMesh("./resources/tv/TV01.obj");
    auto meshes = holubiho::ResourceManager::LoadMeshesFromFile("./resources/tv/TV.obj");
    for (auto mesh : meshes) {
        auto model = tv->AddComponent<holubiho::ModelComponent>();
        model->mesh = mesh;
    }
    auto tmod = tv->GetAllComponents<holubiho::ModelComponent>();
    tmod[0]->drawUUID = true;
    tmod[0]->mat = holubiho::ResourceManager::CreateMaterial("TV_Screen");
    tmod[0]->mat->textures.push_back(holubiho::ResourceManager::GetTexture("resources/tv/TVTextures/4k/Television_BaseColor.png"));
    tmod[0]->mat->textures.push_back(holubiho::ResourceManager::GetTexture("resources/tv/TVTextures/4k/Television_Normal.png"));
    tmod[0]->mat->textures.push_back(filmScript->GetTexture());
    tmod[0]->mat->shader = holubiho::ResourceManager::LoadShaderProgram("TV_shader", "./shaders/vertex/base.vert", "./shaders/fragment/tv_screen.frag");

    tmod[0]->mat->AddInfo("minX", &tv_minX, holubiho::UniformType::Float);
    tmod[0]->mat->AddInfo("maxX", &tv_maxX, holubiho::UniformType::Float);
    tmod[0]->mat->AddInfo("minY", &tv_minY, holubiho::UniformType::Float);
    tmod[0]->mat->AddInfo("maxY", &tv_maxY, holubiho::UniformType::Float);
    auto tvscr = tv->AddScript<holubiho::TVScript>();
    tv->transform->localPosition = glm::vec3(0.270f, 0.720f, -11.142f);
    tv->transform->localRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,1,0));


    auto nextButton = scene->CreateObject("NextButton");
    auto nextModel = nextButton->AddComponent<holubiho::ModelComponent>();
    nextModel->mesh = holubiho::ResourceManager::GetMesh("resources/cube.glb");
    nextModel->mat = holubiho::ResourceManager::CreateMaterial("NextButtonMat");
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
    auto playModel = playButton->AddComponent<holubiho::ModelComponent>();
    playModel->mesh = holubiho::ResourceManager::GetMesh("resources/cube.glb");
    playModel->mat = holubiho::ResourceManager::CreateMaterial("PlayButtonMat");
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
    auto prevModel = prevButton->AddComponent<holubiho::ModelComponent>();
    prevModel->mesh = holubiho::ResourceManager::GetMesh("resources/cube.glb");;
    prevModel->mat = holubiho::ResourceManager::CreateMaterial("PrevButtonMat");
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
    holubiho::ResourceManager::LoadModelsFromFile("resources/low_poly_dining_table.glb", table, false );
    table->transform->localPosition = glm::vec3(0.180f, 0.733f, -11.142f);
    table->transform->localRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1,0,0));
    table->transform->localScale = glm::vec3(0.5f);


    auto SpotlightObj = scene->CreateObject("TVLight");
    auto* SpotlightLight = SpotlightObj->AddComponent<holubiho::LightComponent>();
    SpotlightLight->type        = holubiho::LightType::Spotlight;
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

holubiho::SceneObject* SetupPlane(holubiho::SceneManager* scene, bool* debugPtr) {
    auto planeWrapper = scene->CreateObject("planeWrapper");
    auto plane = scene->CreateObject("plane");
    holubiho::ResourceManager::LoadModelsFromFile("resources/low-poly-biplane/biplane.glb", plane, true);
    auto children = plane->GetChildren();
    plane->transform->localScale = glm::vec3(0.2f);
    glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0,1,0))*glm::angleAxis(glm::radians(-90.0f), glm::vec3(0,0,1));
    plane->transform->Rotate(rot);
    planeWrapper->AddChild(plane);
    auto planeScr = planeWrapper->AddScript<holubiho::BiplaneScript>();
    planeScr->SetPlane(children[0]);
    children[0]->hasClickableParts = true;
    planeScr->SetAirscrew(children[1]);
    planeWrapper->transform->localPosition = glm::vec3(-5.076f, 1.269f, -26.196f);
    planeWrapper->transform->localRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0,1,0));

    std::vector<glm::vec3> planeControlPoints = {
        glm::vec3(0.0f, 20.0f, 10.0f),
        glm::vec3(10.0f, 20.0f, 20.0f),
        glm::vec3(15.0f, 30.0f, 0.0f),
        glm::vec3(20.0f, 40.0f, -20.0f),
        glm::vec3(0.0f, 30.0f, -20.0f),
        glm::vec3(-20.0f, 20.0f, -10.0f),
        glm::vec3(-10.0f, 20.0f, 0.0f),
        glm::vec3(-20.0f, 20.0f, 10.0f)
    };

    std::vector<holubiho::SceneObject*> controlPointObjects;
    int cpIndex = 0;
    for (auto point : planeControlPoints) {
        auto cpObj = scene->CreateObject("PlaneControlPoint" + std::to_string(cpIndex++));
        auto cpModel = cpObj->AddComponent<holubiho::ModelComponent>();
        cpModel->drawUUID = true;
        cpModel->mesh = holubiho::ResourceManager::GetMesh("resources/cube.glb");
        auto cpMat = holubiho::ResourceManager::CreateMaterial("controlPointMat");
        cpMat->ambient   = glm::vec4(0.0f);
        cpMat->diffuse   = glm::vec4(glm::vec3(1, 0, 0), 1);
        cpMat->specular  = glm::vec3(0.0f);
        cpMat->emission  = glm::vec3(1, 0, 0);
        cpModel->mat = cpMat;
        cpObj->transform->localPosition = point;
        cpObj->transform->localScale = glm::vec3(0.5f);
        controlPointObjects.push_back(cpObj);
    }

    // Instantiate the curve
    holubiho::Curve closedCurve = holubiho::Curve::createClosedLoop(planeControlPoints);

    planeScr->SetFlightPath(closedCurve);
    planeScr->SetControlPointObjects(controlPointObjects);
    planeScr->SetDebugPointer(debugPtr);


    return planeWrapper;
}

holubiho::SceneObject* SetupCamera(holubiho::SceneManager* scene, bool* debugPtr) {
    auto* camera = scene->cameraObject;
    if (!camera) {
        std::cerr << "CameraComponent is missing on cameraObject." << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(holubiho::RenderSystem::GetGLFWWindow());
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    auto cameraScript = scene->cameraObject->AddScript<holubiho::CameraScript>();
    scene->cameraObject->transform->localPosition = glm::vec3(0.0f, 00.0f, 10.0f);
    std::vector<glm::vec3> cameraControlPoints = {
        glm::vec3( -6.00f, 25.00f,  -5.00f),
        glm::vec3( 10.26f, 20.00f,  -8.52f),
        glm::vec3( 17.00f, 15.00f, -17.00f),
        glm::vec3( 10.26f, 20.00f, -25.48f),
        glm::vec3( -6.00f, 25.00f, -29.00f),
        glm::vec3(-22.26f, 20.00f, -25.48f),
        glm::vec3(-29.00f, 15.00f, -17.00f),
        glm::vec3(-22.26f, 20.00f,  -8.52f)
    };
    int cpIndex = 0;
    std::vector<holubiho::SceneObject*> cameraControlPointObjects;
    for (auto point : cameraControlPoints) {
        auto cpObj = camera->scene->CreateObject("CameraControlPoint" + std::to_string(cpIndex++));
        auto cpModel = cpObj->AddComponent<holubiho::ModelComponent>();
        cpModel->drawUUID = true;
        cpObj->render = false;
        cpModel->mesh = holubiho::ResourceManager::GetMesh("resources/cube.glb");
        auto cpMat = holubiho::ResourceManager::CreateMaterial("CameraControlPointMat");
        cpMat->ambient   = glm::vec4(0.0f);
        cpMat->diffuse   = glm::vec4(glm::vec3(0, 0, 1), 1);
        cpMat->specular  = glm::vec3(0.0f);
        cpMat->emission  = glm::vec3(0, 0, 1);
        cpModel->mat = cpMat;
        cpObj->transform->localPosition = point;
        cpObj->transform->localScale = glm::vec3(0.5f);
        cameraControlPointObjects.push_back(cpObj);
    }
    cameraScript->SetControlPointObjects(cameraControlPointObjects);
    cameraScript->SetDebugPointer(debugPtr);
    return  camera;
}