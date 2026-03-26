#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <iostream>

struct Character {
    GLuint     TextureID;   // ID текстуры глифа
    int        Width;       // ширина
    int        Height;      // высота
    int        BearingX;    // смещение по X от baseline
    int        BearingY;    // смещение по Y от baseline
    long       Advance;     // расстояние до следующего символа (в 1/64 px)
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    // Загрузка шрифта (только английский ASCII для начала)
    bool LoadFont(const std::string& fontPath, unsigned int fontSize);

    // Рендер текста
    // x, y — позиция baseline (нижняя линия текста)
    void RenderText(const std::string& text, float x, float y, 
                    float scale = 1.0f, 
                    float r = 1.0f, float g = 1.0f, float b = 1.0f);

private:
    std::map<char, Character> Characters;
    FT_Library  ft;
    FT_Face     face;
};