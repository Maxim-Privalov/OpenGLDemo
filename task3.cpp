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
#include "app/extensions/fractals.hpp"
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
    int dynamic_rate = 0;
    int dynamic_step = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::mt19937 PLrandomGen(rd()); 

    std::map<std::string, std::string> rules = { {"F", "FF[+F][F][-F]"} };
    std::string start_token = "F";
    double degree = 0;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
        int offsetX = 0;
        int offsetY = 0;

        Fractal frac1 = Fractal(Vertex(500, 600), dynamic_rate, M_PI);
        auto instructions = frac1.create_Lsystem(start_token, rules);

        frac1.draw_Lsystem_tree(instructions, 19, dynamic_step*10);

        int octS = 150;

        Fractal frac_P = Fractal(Vertex(300, 800), 7, -M_PI);
        frac_P.draw_pythagorean_tree(80, true);
        

        Fractal frac_PL = Fractal(Vertex(700, 800), 7, -M_PI);
        frac_PL.draw_pythagorean_tree(80, false, std::make_pair(M_PI/4, M_PI/4+M_PI/10), PLrandomGen);

        
        std::map<std::string, std::string> rules = { {"F", "F[+F][-F]"} };
        std::string start_token = "F";

        Fractal frac_Lsystem = Fractal(Vertex(200, 300), 7, 0);
        auto instructions_example = frac_Lsystem.create_Lsystem(start_token, rules);
        frac_Lsystem.draw_Lsystem_tree(instructions_example, 80, 480);

        if (dynamic_rate > 4) {
            dynamic_rate = 0;
            dynamic_step = 0;
        }
        if (static_cast<int>(std::trunc(glfwGetTime() * 100)) != time_millisec_timer) {
            if (dynamic_step < 480 / 10) {
                dynamic_step += 1;
            }
            degree += 0.01;
            time_millisec_timer = static_cast<int>(std::round(glfwGetTime() * 100));
        }

        if (static_cast<int>(std::trunc(glfwGetTime())) != time_sec_timer) {
            if (dynamic_rate < 4) {
                dynamic_rate += 1;
            }
            time_sec_timer = static_cast<int>(std::round(glfwGetTime()));
        }
        
        if (degree > M_PI*2) {
            degree = 0;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}