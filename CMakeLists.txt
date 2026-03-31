cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(Viewer)

include(FetchContent)

# --- GLAD (OpenGL Loader) ---
FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG        v0.1.36 # Стабільна версія для OpenGL 3.3+
)
# Налаштовуємо GLAD перед завантаженням
set(GLAD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glad)

# --- GLFW (Window Management) ---
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- GLM (Mathematics) ---
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# --- ImGui (UI) ---
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
        stduuid
        GIT_REPOSITORY https://github.com/mariusbancila/stduuid.git
        GIT_TAG        v1.2.3
)

# Make the library available to the build
FetchContent_MakeAvailable(stduuid)
# Створення бібліотеки ImGui
add_library(imgui_lib STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        components/Component.cpp
        components/Component.h
)
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

# --- Основний проект ---
add_executable(viewer main.cpp test_models/birds.c
        core/InputHandler.cpp
        core/InputHandler.h
        BiBuild.h
        core/SceneManager.cpp
        core/SceneManager.h
        core/SceneObject.cpp
        core/SceneObject.h
        components/MeshComponent.cpp
        components/MeshComponent.h
        components/LightComponent.cpp
        components/LightComponent.h
        components/TransformComponent.cpp
        components/TransformComponent.h
        components/CameraComponent.cpp
        components/CameraComponent.h
        core/ResourceManager.cpp
        core/ResourceManager.h
        core/Mesh.cpp
        core/Mesh.h
        core/RenderSystem.cpp
        core/RenderSystem.h
        helper_functions/helper.h
        )

# Знаходимо системний OpenGL
find_package(OpenGL REQUIRED)

# Лінкуємо ВСЕ разом
target_link_libraries(viewer PRIVATE
        imgui_lib
        glfw
        glad          # Додано GLAD
        glm::glm      # Додано GLM
        OpenGL::GL
        stduuid
)