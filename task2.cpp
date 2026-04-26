#define _USE_MATH_DEFINES

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
    // FPS
    double targetFPS = 120.0;
    double timePerFrame = 1.0 / targetFPS;

    // рандом
    std::random_device rd;
    std::mt19937 gen(rd()); 

    std::uniform_int_distribution<> pos_dist(1, 400); 
    std::uniform_int_distribution<> size_dist(5, 300); 
    std::uniform_int_distribution<> rotate_dist(1, 360);
    std::uniform_int_distribution<> color_dist(1, 255);
    std::vector<std::tuple<Vertex, std::pair<int, int>, int, Color, Color>> square_rd_pool;
    

    for (std::size_t i = 0; i < 25; i++) {
        square_rd_pool.push_back(std::make_tuple(
            Vertex(pos_dist(gen), pos_dist(gen)),
            std::make_pair<int, int>(size_dist(gen), size_dist(gen)),
            rotate_dist(gen),
            Color(static_cast<float>(color_dist(gen)) / 255, static_cast<float>(color_dist(gen)) / 255, static_cast<float>(color_dist(gen)) / 255, 1.0f),
            Color(static_cast<float>(color_dist(gen)) / 255, static_cast<float>(color_dist(gen)) / 255, static_cast<float>(color_dist(gen)) / 255, 0.5f)
        ));
    }

    int t = 0;
    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(2.0f);
        int offsetX = 0;
        int offsetY = 0;

        int sX = static_cast<double>(t)/(120 * 10)*(500 - 10);
        int sY = static_cast<double>(t)/(120 * 10)*(500 - 10);
        int sWidth = 200;
        int sHeight = 200;

        for (std::size_t i = 0; i < 25; i++) {
            auto rect_random = Rect(std::get<0>(square_rd_pool[i]), std::get<0>(std::get<1>(square_rd_pool[i])), std::get<1>(std::get<1>(square_rd_pool[i])));
            rect_random.rotate(std::get<2>(square_rd_pool[i]));
            rect_random.strokeColor(std::get<3>(square_rd_pool[i]));
            rect_random.fillColor(std::get<4>(square_rd_pool[i]));
            rect_random.setCropBorder(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
            rect_random.render();

        }
        
        auto rectb = Rect(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
        rectb.strokeColor(Color(1, 0, 0, 1));
        rectb.render();

        t++;
        if ((t/120) > 10) {
            t = 0;
        }

        // Для Лиана-Барски
        
        /*
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
        
        
        //Для Сазерленда-Коэна
        int step = 400 * t/10000;
        
        auto r = Rect(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
        r.strokeColor(Color(1, 0, 0, 1));
        r.render();
        
    
        for (int s = 0; s < h / 2; s += 102 - 100*double(t)/10000) {
            auto r = Rect(Vertex(offsetX + s, offsetY + s), w - s*2, h - s*2);
            r.setCropBorder(Vertex(sX, sY), Vertex(sX + sWidth, sY + sHeight));
            r.render();
        }
        */

        while (glfwGetTime() - startTime < timePerFrame) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}