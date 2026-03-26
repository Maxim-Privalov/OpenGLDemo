#define _USE_MATH_DEFINES
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include "primitives.cpp"
#include "interface.cpp"


int main() {
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });

    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(600, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

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

    double t = 0;
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
        int offsetX = 300;
        int offsetY = 300;
        std::vector<Vertex> line = {Vertex(20, 80), Vertex(100, 80)};
        drawLine(line[0], line[1], Color(0.5f, 0.5f, 1.0f, 1.0f));
        
        Vertex center_vertex_line = getCenterVertex(line[0], line[1]);
        std::vector<Vertex> rotated_line = rotateVertex(line, center_vertex_line, M_PI/4);
        drawLine(rotated_line[0], rotated_line[1], Color(1, 1, 1, 1));

        Vertex mousePointVertex = {interface.getClickPosX(), interface.getClickPosY()};
        //std::vector<Vertex> rotated_rect = rotateVertex(rect, mousePointVertex, rotation_t);
        //std::cout << rotated_line[0] << rotated_line[1];

        Rect rect = {mousePointVertex, 100, 140};
        rect.rotate(M_PI/6);
        rect.strokeColor(Color(0.5f, 1.0f, 0.5f, 1.0f));\
        rect.rotate(t);
        rect.render();

        Rect rect1 = {Vertex(250, 100), 70, 140};
        rect1.rotate(M_PI/6);
        rect1.strokeColor(Color(0.5f, 1.0f, 0.5f, 1.0f));
        rect1.render();

        t += 0.001;
        glfwPollEvents();
        glfwSwapBuffers(window);   
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}