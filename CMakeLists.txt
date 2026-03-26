cmake_minimum_required(VERSION 3.10)
project(OpenGL_Project LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BOOST_ROOT "C:/boost_1_90_0")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_DEBUG ON)        

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "" FORCE)
set(GLFW_INSTALL        OFF CACHE INTERNAL "" FORCE)

add_subdirectory(external/glfw)

add_executable(${PROJECT_NAME}
    main.cpp
    glad/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    glad/include
    ${BOOST_ROOT}
)

target_link_libraries(${PROJECT_NAME}
    glfw
    opengl32
)

