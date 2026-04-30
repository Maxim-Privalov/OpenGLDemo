#define _USE_MATH_DEFINES
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <utility>
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>
#include "app/primitives/2dshapes.hpp"
#include "app/primitives/triangulation.hpp"
#include "app/extensions/polygons.hpp"
#include "app/interface.hpp"


int main() {
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // ← Это ключ!

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Проверка, что мы действительно получили legacy-контекст
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_LINE_SMOOTH_HINT,   GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h); // Область вывода

    // Идём на матрицу проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // сброс
    
    // ортографическая проекция
    glOrtho(0, w, h, 0, -1.0, 1.0); 

    // идём на матрицу вида модели
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // сброс

    auto& interface = GUInterface::getInstance();
    
    // Прикрепляем указатель на обработчик к окну
    glfwSetWindowUserPointer(window, &interface);
    // Устанавливаем статические методы как callback'и
    glfwSetMouseButtonCallback(window, GUInterface::mouse_button_callback);
    // FPS
    int time_sec_timer = 0;
    int time_millisec_timer = 0;

    std::random_device rd;
    std::mt19937 gen1(rd());
    std::uniform_int_distribution<> color_dist(0, 255);

    std::vector<Vertex> poly1 = {Vertex(0, 0), Vertex(50, -20), Vertex(70, 50), Vertex(70, 80), Vertex(40, 90), Vertex(20, 120), Vertex(0, 80)};
    poly1 = parallelMove(poly1, Vertex(0, 0), Vertex(200, 200));

    auto ell = Ellipsis(Vertex(500, 250), 200, 400);
    auto polystar = getRdVertexStarlike(ell, 7, gen1);

    auto trPoly = triangulate(polystar);
    for (auto& tr : trPoly) {
        tr.fillColor(Color(color_dist(gen1), color_dist(gen1), color_dist(gen1), 1));
    }

    std::vector<Vertex> vp = {Vertex(0, 0), Vertex(200, -50), Vertex(200, 0), Vertex(150, 150), Vertex(200, 150), Vertex(50, 200), Vertex(0, 50)};
    std::vector<Vertex> vpIn = {Vertex(50, 50), Vertex(100, 50), Vertex(75, 100), Vertex(25, 100)};
    vp = parallelMove(vp, vp[0], Vertex(200, 500));
    vpIn = parallelMove(vpIn, vpIn[0], Vertex(300, 550));

    auto trAdv = triangulate(vp, vpIn);
    for (auto& tr : trAdv) {
        tr.fillColor(Color(color_dist(gen1), color_dist(gen1), color_dist(gen1), 1));
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
    
        drawArea(parallelMove(polystar, polystar[0], Vertex(300, 280)), Color(140, 222, 10, 1), std::nullopt, 0, w, 0, h);
        drawArea(vp, Color(140, 222, 10, 1), std::nullopt, 0, w, 0, h);
        drawArea(vpIn, Color(140, 222, 10, 1), std::nullopt, 0, w, 0, h);


        ell.render();
        for (auto& tr : trPoly) {
            tr.render();
        }

        for (auto& tr : trAdv) {
            tr.render();
        }
        


        if (static_cast<int>(std::trunc(glfwGetTime() * 100)) != time_millisec_timer) {
            time_millisec_timer = static_cast<int>(std::round(glfwGetTime() * 100));
        }

        if (static_cast<int>(std::trunc(glfwGetTime())) != time_sec_timer) {
            time_sec_timer = static_cast<int>(std::round(glfwGetTime()));
        }
        

        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}