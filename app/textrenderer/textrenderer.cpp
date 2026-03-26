#include "textrenderer.hpp"

TextRenderer::TextRenderer()
{
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
}

TextRenderer::~TextRenderer()
{
    for (auto& ch : Characters)
        glDeleteTextures(1, &ch.second.TextureID);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

bool TextRenderer::LoadFont(const std::string& fontPath, unsigned int fontSize)
{
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font " << fontPath << std::endl;
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   // важно для 1-канальных текстур

    // Загружаем символы ASCII 0..127 (английский + стандартные символы)
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph " << c << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,                              // или GL_RED в новых версиях
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_ALPHA,                              // или GL_RED
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            face->glyph->advance.x
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, float r, float g, float b)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor4f(r, g, b, 1.0f);

    for (char c : text)
    {
        if (Characters.find(c) == Characters.end())
            continue;

        Character ch = Characters[c];

        // Исправленная позиция для glOrtho(0, w, h, 0, -1, 1) — Y вниз
        float xpos = x + ch.BearingX * scale;
        float ypos = y + (ch.Height - ch.BearingY) * scale;   // ← главное изменение здесь

        float w = ch.Width * scale;
        float h = ch.Height * scale;

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos,     ypos - h);   // левый верх
            glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos - h);   // правый верх
            glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos);       // правый низ
            glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos,     ypos);       // левый низ
        glEnd();

        // Переход к следующему символу
        x += (ch.Advance >> 6) * scale;
    }

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}