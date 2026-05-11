cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(Project C CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}")
include(FetchContent)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG        v0.1.36
)

set(GLAD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

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


FetchContent_MakeAvailable(stduuid)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.5
)

set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

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
target_include_directories(imgui_lib PRIVATE ${glfw_SOURCE_DIR}/include)
target_link_libraries(imgui_lib PRIVATE glfw)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

if(WIN32)
    FetchContent_Declare(
            vlc_sdk
            URL https://www.nuget.org/api/v2/package/VideoLAN.LibVLC.Windows/3.0.20
    )
    FetchContent_MakeAvailable(vlc_sdk)

    set(VLC_ROOT "${vlc_sdk_SOURCE_DIR}/build/x64")
    set(VLC_INCLUDE_DIRS "${VLC_ROOT}/include")
    set(VLC_LIBRARY_DIRS "${VLC_ROOT}")
    set(VLC_LIBRARIES libvlc libvlccore)

    file(GLOB VLC_DLLS "${VLC_ROOT}/*.dll")
    file(COPY ${VLC_DLLS} DESTINATION ${CMAKE_SOURCE_DIR})
file(COPY "${VLC_ROOT}/plugins" DESTINATION ${CMAKE_SOURCE_DIR})
else()
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
        GIT_TAG        VER-2-13-2
)
FetchContent_MakeAvailable(freetype)

add_executable(project_pgr main.cpp
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
        core/Texture.cpp
        core/Texture.h
        core/Material.cpp
        core/Material.h
        core/stb_image.cpp
        gui/Debug.cpp
        gui/Debug.h
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
        ObjectScripts/TVScript.cpp
        ObjectScripts/TVScript.h
        setup.h
        ObjectScripts/BiplaneScript.cpp
        ObjectScripts/BiplaneScript.h
        setup.cpp
)

find_package(OpenGL REQUIRED)

target_link_libraries(project_pgr PRIVATE
        imgui_lib
        glfw
        glad
        glm::glm
        OpenGL::GL
        stduuid
        assimp
        ${VLC_LIBRARIES}
        freetype
)



target_include_directories(project_pgr PRIVATE ${stb_SOURCE_DIR}
        ${VLC_INCLUDE_DIRS})

target_link_directories(project_pgr PRIVATE ${VLC_LIBRARY_DIRS})
