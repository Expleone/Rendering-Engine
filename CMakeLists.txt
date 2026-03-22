cmake_minimum_required(VERSION 3.14)
project(Viewer)

# 1. Підключення FetchContent
include(FetchContent)

# 2. Завантаження GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4 # Або остання стабільна версія
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 3. Завантаження ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.92.6
)
FetchContent_MakeAvailable(imgui)

# 4. Створення бібліотеки ImGui (оскільки вона не має власного CMakeLists)
add_library(imgui_lib STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Додаємо шляхи до заголовних файлів ImGui
target_include_directories(imgui_lib PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# 5. Створення вашого виконуваного файлу
add_executable(viewer main.cpp)

# 6. Лінкування всього разом
# OpenGL зазвичай є в системі, тому шукаємо його:
find_package(OpenGL REQUIRED)

target_link_libraries(viewer PRIVATE imgui_lib glfw OpenGL::GL)