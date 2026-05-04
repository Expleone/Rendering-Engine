cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(Viewer)

include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.3 # Вкажіть потрібну версію або хеш
)

# Вимкнення зайвих функцій для прискорення збірки
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# Створення бібліотеки ImGui
add_library(imgui_lib STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

if(WIN32)
    # Fetch precompiled VLC Windows SDK directly from VideoLAN
    FetchContent_Declare(
            vlc_sdk
            URL https://download.videolan.org/pub/videolan/vlc/3.0.20/win64/vlc-3.0.20-win64.zip
    )
    FetchContent_MakeAvailable(vlc_sdk)

    set(VLC_INCLUDE_DIRS ${vlc_sdk_SOURCE_DIR}/sdk/include)
    set(VLC_LIBRARY_DIRS ${vlc_sdk_SOURCE_DIR}/sdk/lib)
    set(VLC_LIBRARIES libvlc libvlccore)

    # Automatically copy the required DLLs to the build directory so the executable can run
    file(GLOB VLC_DLLS "${vlc_sdk_SOURCE_DIR}/*.dll")
    file(COPY ${VLC_DLLS} DESTINATION ${CMAKE_BINARY_DIR})

    # Also copy the plugins folder which VLC requires at runtime
    file(COPY "${vlc_sdk_SOURCE_DIR}/plugins" DESTINATION ${CMAKE_BINARY_DIR})
else()
    # UNIX platforms: Precompiled binaries are highly volatile due to glibc/framework variations.
    # Package managers or PkgConfig remain the required standard.
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(VLC REQUIRED libvlc)
endif()

set(FT_DISABLE_ZLIB ON CACHE BOOL "Disable ZLIB")
set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable BZIP2")
set(FT_DISABLE_PNG ON CACHE BOOL "Disable PNG")
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable HarfBuzz")
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable Brotli")

FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG        VER-2-13-2 # Specify the desired release tag
)
FetchContent_MakeAvailable(freetype)

# --- Основний проект ---
add_executable(viewer main.cpp test_models/birds.c
        core/InputHandler.cpp
        core/InputHandler.h
        BiBuild.h
        core/SceneManager.cpp
        core/SceneManager.h
        core/SceneObject.cpp
        core/SceneObject.h
        components/ModelComponent.cpp
        components/ModelComponent.h
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
        core/ShaderProgram.cpp
        core/ShaderProgram.h
        core/VertexBuffer.cpp
        core/VertexBuffer.h
        core/VertexArray.cpp
        core/VertexArray.h
        core/IndexBuffer.cpp
        core/IndexBuffer.h
        core/UniformBuffer.cpp
        core/UniformBuffer.h
        core/MeshAnim.cpp
        core/MeshAnim.h
        test_models/cube.cpp
        test_models/cube.h
        core/Texture.cpp
        core/Texture.h
        core/Material.cpp
        core/Material.h
        core/stb_image.cpp
        gui/ModelLoadGUI.cpp
        gui/ModelLoadGUI.h
        ObjectScripts/ObjectScript.cpp
        ObjectScripts/ObjectScript.h
        ObjectScripts/SkyboxScript.cpp
        ObjectScripts/SkyboxScript.h
        ObjectScripts/CameraScript.cpp
        ObjectScripts/CameraScript.h
        core/Time.cpp
        core/Time.h
        core/FrameBuffer.cpp
        core/FrameBuffer.h
        ObjectScripts/OutlineScript.cpp
        ObjectScripts/OutlineScript.h
        ObjectScripts/FilmScript.cpp
        ObjectScripts/FilmScript.h
        core/Curve.cpp
        core/Curve.h
        core/TextGenerator.cpp
        core/TextGenerator.h
        ObjectScripts/TextScript.cpp
        ObjectScripts/TextScript.h
)

#add_executable(birds c.cpp test_models/birds.c
#        c.cpp)

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
        assimp
        ${VLC_LIBRARIES}
        freetype
)



target_include_directories(viewer PRIVATE ${stb_SOURCE_DIR}
        ${VLC_INCLUDE_DIRS})

target_link_directories(viewer PRIVATE ${VLC_LIBRARY_DIRS})

#target_link_libraries(birds PRIVATE
#        imgui_lib
#        glfw
#        glad          # Додано GLAD
#        glm::glm      # Додано GLM
#        OpenGL::GL
#        stduuid
#
#)