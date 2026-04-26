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
#include "app/primitives/curvensmooth.hpp"
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
    std::mt19937 gen2(rd());


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
        int offsetX = 0;
        int offsetY = 0;
        

        auto ellip = Ellipsis(Vertex(400, 300), 300, 250);
        auto rect = Rect(Vertex(400, 300), 300, 250);
        rect.move(Vertex(400, 300), Vertex(400 - 300 / 2, 300 - 250 / 2));

        auto rd_poligon_vertices = getRdVertexOnEllipsis(ellip, 10, gen1);
        drawArea(rd_poligon_vertices, Color(255, 212, 12, 1), Color(212, 148, 28, 1), 0, 800, 0, 800);

        auto ellip_moved = Ellipsis(parallelMove(ellip.getOctant().to_vector(), ellip.getPosition(), Vertex(400, 600)));

        auto rd_star_vertices = getRdVertexStarlike(ellip_moved, 10, gen2);
        drawArea(rd_star_vertices, Color(255, 212, 12, 1), Color(212, 148, 28, 1), 0, 800, 0, 800);

        auto r1 = Rect(Vertex(600, 300), 100, 300);
        r1.strokeColor(Color(40, 40, 40, 1));
        r1.render();

        auto butter_vert = getButterflyPolygon(r1, gen1);
        drawArea(butter_vert, Color(255, 212, 12, 1), Color(212, 148, 28, 1), 0, 800, 0, 800);

        auto r2 = Rect(Vertex(600, 650), 300, 300);
        r2.strokeColor(Color(40, 40, 40, 1));
        r2.render();
        
        auto spiral_vert = getSpiralPolygon(r2, M_PI*5, M_PI/6, gen1);
        drawArea(spiral_vert, Color(255, 212, 12, 1), std::nullopt, 0, 800, 0, 800);

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