cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === GLAD как отдельная статическая библиотека ===
add_library(glad STATIC glad/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

target_include_directories(glad PRIVATE glad/include)

# === Основной исполняемый файл ===
add_executable(${PROJECT_NAME}
    task6.cpp
    app/textrenderer/textrenderer.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    glad/include
)

find_package(glfw3 CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(boost CONFIG REQUIRED)

# === Линковка ===
target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glfw
    opengl32
    Freetype::Freetype
    Boost::boost
    # freetype-gl
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()