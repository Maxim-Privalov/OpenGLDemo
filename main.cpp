#define _USE_MATH_DEFINES

#define _USE_MATH_DEFINES

#define GLFW_INCLUDE_NONE          // Крайне важно!
#include <glad/glad.h>             // Сначала glad
#include <GLFW/glfw3.h>

#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include "app/primitives/2dshapes.hpp"
#include "app/textrenderer/textrenderer.hpp"
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

    GLFWwindow* window = glfwCreateWindow(600, 600, "LearnOpenGL", nullptr, nullptr);
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

    TextRenderer text;
    text.LoadFont("C:/Windows/Fonts/arial.ttf", 36);

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

    //init_text();

    int t = 0;
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
        int offsetX = 0;
        int offsetY = 0;
        
        //render_text("page 1", 500, 30);

        int sX = 50;
        int sY = 50;
        int sWidth = 300;
        int sHeight = 300;
        

        text.RenderText("Page 1", 500, 40.0f, 0.8f, 1.0f, 1.0f, 1.0f);

        // Для Лиана-Барски
        auto r = Rect(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
        r.strokeColor(Color(1, 0, 0, 1));
        r.render();

        int c = h / (2*25);

        std::vector<int> directions;
        for (int i = 0; i < c; i++) {
            directions.push_back((i % 2 == 0) ? 1 : -1);
        }

        int i = 0;
        for (int s = 0; s < h / 2; s += 25) {
             auto r = Rect(Vertex(offsetX + s, offsetY + s), w - s*2, h - s*2);
             r.rotate(directions[i]*2*M_PI*t/10000);
             r.setCropBorder(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
             r.render();
             i++;
        }
        
        /*
         Для Сазерленда-Коэна
        int step = 400 * t/10000;
        
        auto r = Rect(Vertex(sX + step, sY + step), Vertex(sX + sWidth + step, sY + sHeight + step));
        r.strokeColor(Color(1, 0, 0, 1));
        r.render();
        */
    
        // for (int s = 0; s < h / 2; s += 102 - 100*double(t)/10000) {
        //     auto r = Rect(Vertex(offsetX + s, offsetY + s), w - s*2, h - s*2);
        //     r.setCropBorder(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
        //     r.render();
        // }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
        t++;
        if (t >= 10000) {
            t = 0;
        }
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}