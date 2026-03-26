#ifndef SIMPLE_TEXT_H
#define SIMPLE_TEXT_H

#include <GLFW/glfw3.h>
#include <string>

class SimpleText {
public:
    static void drawText(float x, float y, const std::string& text, float scale = 1.0f, 
                    float colorR = 1.0f, float colorG = 1.0f, float colorB = 1.0f) {
    // Сохраняем ВСЕ текущие настройки
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    // Сохраняем состояния
    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lighting = glIsEnabled(GL_LIGHTING);
    
    // Устанавливаем ортогональную проекцию для текста
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    int width, height;
    GLFWwindow* window = glfwGetCurrentContext();
    glfwGetFramebufferSize(window, &width, &height);
    glOrtho(0, width, 0, height, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Отключаем для текста
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    // Устанавливаем цвет
    glColor3f(colorR, colorG, colorB);
    
    // Рисуем текст
    drawTextShapes(x, y, text, scale);
    
    // Восстанавливаем ВСЕ настройки точно в обратном порядке
    if (depthTest) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    
    if (lighting) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
    
private:
     static void drawTextShapes(float x, float y, const std::string& text, float scale) {
        float charWidth = 12.0f * scale;
        float currentX = x;
        
        for (size_t i = 0; i < text.length(); i++) {
            drawChar(text[i], currentX, y, scale);
            currentX += charWidth;
        }
    }
    
    static void drawChar(char c, float x, float y, float scale) {
        float size = 10.0f * scale;
        
        switch (c) {
            // Uppercase letters
            case 'A': drawA(x, y, size); break;
            case 'B': drawB(x, y, size); break;
            case 'C': drawC(x, y, size); break;
            case 'D': drawD(x, y, size); break;
            case 'E': drawE(x, y, size); break;
            case 'F': drawF(x, y, size); break;
            case 'G': drawG(x, y, size); break;
            case 'H': drawH(x, y, size); break;
            case 'I': drawI(x, y, size); break;
            case 'J': drawJ(x, y, size); break;
            case 'K': drawK(x, y, size); break;
            case 'L': drawL(x, y, size); break;
            case 'M': drawM(x, y, size); break;
            case 'N': drawN(x, y, size); break;
            case 'O': drawO(x, y, size); break;
            case 'P': drawP(x, y, size); break;
            case 'Q': drawQ(x, y, size); break;
            case 'R': drawR(x, y, size); break;
            case 'S': drawS(x, y, size); break;
            case 'T': drawT(x, y, size); break;
            case 'U': drawU(x, y, size); break;
            case 'V': drawV(x, y, size); break;
            case 'W': drawW(x, y, size); break;
            case 'X': drawX(x, y, size); break;
            case 'Y': drawY(x, y, size); break;
            case 'Z': drawZ(x, y, size); break;
            
            // Lowercase letters
            case 'a': drawa(x, y, size); break;
            case 'b': drawb(x, y, size); break;
            case 'c': drawc(x, y, size); break;
            case 'd': drawd(x, y, size); break;
            case 'e': drawe(x, y, size); break;
            case 'f': drawf(x, y, size); break;
            case 'g': drawg(x, y, size); break;
            case 'h': drawh(x, y, size); break;
            case 'i': drawi(x, y, size); break;
            case 'j': drawj(x, y, size); break;
            case 'k': drawk(x, y, size); break;
            case 'l': drawl(x, y, size); break;
            case 'm': drawm(x, y, size); break;
            case 'n': drawn(x, y, size); break;
            case 'o': drawo(x, y, size); break;
            case 'p': drawp(x, y, size); break;
            case 'q': drawq(x, y, size); break;
            case 'r': drawr(x, y, size); break;
            case 's': draws(x, y, size); break;
            case 't': drawt(x, y, size); break;
            case 'u': drawu(x, y, size); break;
            case 'v': drawv(x, y, size); break;
            case 'w': draww(x, y, size); break;
            case 'x': drawx(x, y, size); break;
            case 'y': drawy(x, y, size); break;
            case 'z': drawz(x, y, size); break;
            
            // Numbers
            case '0': draw0(x, y, size); break;
            case '1': draw1(x, y, size); break;
            case '2': draw2(x, y, size); break;
            case '3': draw3(x, y, size); break;
            case '4': draw4(x, y, size); break;
            case '5': draw5(x, y, size); break;
            case '6': draw6(x, y, size); break;
            case '7': draw7(x, y, size); break;
            case '8': draw8(x, y, size); break;
            case '9': draw9(x, y, size); break;
            
            // Special characters
            case ' ': break; // Space
            case '.': drawDot(x, y, size); break;
            case ',': drawComma(x, y, size); break;
            case '!': drawExclamation(x, y, size); break;
            case '?': drawQuestion(x, y, size); break;
            case ':': drawColon(x, y, size); break;
            case ';': drawSemicolon(x, y, size); break;
            case '-': drawDash(x, y, size); break;
            case '_': drawUnderscore(x, y, size); break;
            case '+': drawPlus(x, y, size); break;
            case '=': drawEquals(x, y, size); break;
            case '/': drawSlash(x, y, size); break;
            case '\\': drawBackslash(x, y, size); break;
            case '(': drawParenthesisLeft(x, y, size); break;
            case ')': drawParenthesisRight(x, y, size); break;
            case '[': drawBracketLeft(x, y, size); break;
            case ']': drawBracketRight(x, y, size); break;
            case '<': drawAngleLeft(x, y, size); break;
            case '>': drawAngleRight(x, y, size); break;
            
            default: drawUnknown(x, y, size); break;
        }
    }
    
    // UPPERCASE LETTERS
    static void drawA(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + size/2, y + size);
        glVertex2f(x + size, y);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x + size/4, y + size/2);
        glVertex2f(x + 3*size/4, y + size/2);
        glEnd();
    }
    
    static void drawB(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + 2*size/3, y + size);
        glVertex2f(x + size, y + 3*size/4);
        glVertex2f(x + 2*size/3, y + size/2);
        glVertex2f(x, y + size/2);
        glVertex2f(x + 2*size/3, y + size/2);
        glVertex2f(x + size, y + size/4);
        glVertex2f(x + 2*size/3, y);
        glVertex2f(x, y);
        glEnd();
    }
    
    static void drawC(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawD(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + 2*size/3, y + size);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + 2*size/3, y);
        glVertex2f(x, y);
        glEnd();
    }
    
    static void drawE(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + 2*size/3, y + size/2);
        glEnd();
    }
    
    static void drawF(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + 2*size/3, y + size/2);
        glEnd();
    }
    
    static void drawG(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size/2, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void drawH(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y); glVertex2f(x, y + size);
        glVertex2f(x + size, y); glVertex2f(x + size, y + size);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void drawI(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y); glVertex2f(x + size/2, y + size);
        glVertex2f(x, y); glVertex2f(x + size, y);
        glVertex2f(x, y + size); glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawJ(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size/2, y + size);
        glVertex2f(x + size/2, y);
        glVertex2f(x, y);
        glEnd();
    }
    
    static void drawK(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y); glVertex2f(x, y + size);
        glVertex2f(x + size, y + size); glVertex2f(x, y + size/2);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawL(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawM(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawN(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawO(float x, float y, float size) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawP(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + 2*size/3, y + size);
        glVertex2f(x + size, y + 3*size/4);
        glVertex2f(x + 2*size/3, y + size/2);
        glVertex2f(x, y + size/2);
        glEnd();
    }
    
    static void drawQ(float x, float y, float size) {
        drawO(x, y, size);
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawR(float x, float y, float size) {
        drawP(x, y, size);
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawS(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawT(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size/2, y + size);
        glVertex2f(x + size/2, y);
        glEnd();
    }
    
    static void drawU(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawV(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawW(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawX(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawY(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size/2, y);
        glEnd();
    }
    
    static void drawZ(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    // LOWERCASE LETTERS (simplified versions)
    static void drawa(float x, float y, float size) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x + size/4, y + size/2);
        glVertex2f(x + 3*size/4, y + size/2);
        glVertex2f(x + 3*size/4, y);
        glVertex2f(x + size/4, y);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x + 3*size/4, y + size/2);
        glVertex2f(x + 3*size/4, y + size);
        glEnd();
    }
    
    static void drawb(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y); glVertex2f(x, y + size);
        glEnd();
        drawc(x, y, size);
    }
    
    static void drawc(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size/4);
        glVertex2f(x + size/2, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y + size);
        glEnd();
    }
    
    static void drawd(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size, y); glVertex2f(x + size, y + size);
        glEnd();
        drawc(x, y, size);
    }
    
    static void drawe(float x, float y, float size) {
        drawc(x, y, size);
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size/2, y + size/2);
        glEnd();
    }
    
    static void drawf(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y + size);
        glVertex2f(x, y + 3*size/4);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void drawg(float x, float y, float size) {
        drawe(x, y, size);
        glBegin(GL_LINES);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y - size/3);
        glEnd();
    }
    
    static void drawh(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y); glVertex2f(x, y + size);
        glEnd();
        drawn(x, y, size);
    }
    
    static void drawi(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y + size/4);
        glVertex2f(x + size/2, y + size);
        glEnd();
        glBegin(GL_POINTS);
        glVertex2f(x + size/2, y + size/8);
        glEnd();
    }
    
    static void drawj(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y + size/4);
        glVertex2f(x + size/2, y - size/3);
        glEnd();
        glBegin(GL_POINTS);
        glVertex2f(x + size/2, y + size/8);
        glEnd();
    }
    
    static void drawk(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y); glVertex2f(x, y + size);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y + size);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawl(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y); glVertex2f(x + size/2, y + size);
        glEnd();
    }
    
    static void drawm(float x, float y, float size) {
        drawn(x, y, size);
        drawn(x + size/2, y, size/2);
    }
    
    static void drawn(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawo(float x, float y, float size) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawp(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y - size/3); glVertex2f(x, y + size);
        glEnd();
        drawo(x, y, size);
    }
    
    static void drawq(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size, y - size/3); glVertex2f(x + size, y + size);
        glEnd();
        drawo(x, y, size);
    }
    
    static void drawr(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void draws(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawt(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y); glVertex2f(x + size/2, y + size);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void drawu(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawv(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void draww(float x, float y, float size) {
        drawm(x, y, size);
    }
    
    static void drawx(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawy(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size/2, y + size/2);
        glVertex2f(x + size/2, y - size/3);
        glEnd();
    }
    
    static void drawz(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    // NUMBERS
    static void draw0(float x, float y, float size) { drawO(x, y, size); }
    
    static void draw1(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y + size);
        glEnd();
    }
    
    static void draw2(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x, y + size/2);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void draw3(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x, y + size/2);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glEnd();
    }
    
    static void draw4(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2); glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y + size/2); glVertex2f(x + size, y);
        glVertex2f(x, y + size); glVertex2f(x, y + size/2);
        glEnd();
    }
    
    static void draw5(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glEnd();
    }
    
    static void draw6(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x, y + size/2);
        glEnd();
    }
    
    static void draw7(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glVertex2f(x + size/2, y);
        glEnd();
    }
    
    static void draw8(float x, float y, float size) {
        drawO(x, y, size);
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void draw9(float x, float y, float size) {
        draw6(x, y, size);
        // Mirror of 6
    }
    
    // SPECIAL CHARACTERS
    static void drawDot(float x, float y, float size) {
        glBegin(GL_POINTS);
        glVertex2f(x + size/2, y);
        glEnd();
    }
    
    static void drawComma(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y - size/4);
        glEnd();
    }
    
    static void drawExclamation(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x + size/2, y); glVertex2f(x + size/2, y + 3*size/4);
        glEnd();
        drawDot(x, y + 7*size/8, size);
    }
    
    static void drawQuestion(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size/2, y + size);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size/2, y + size/2);
        glEnd();
        drawDot(x + size/2, y, size);
    }
    
    static void drawColon(float x, float y, float size) {
        drawDot(x, y + size/4, size/2);
        drawDot(x, y + 3*size/4, size/2);
    }
    
    static void drawSemicolon(float x, float y, float size) {
        drawComma(x, y, size);
        drawDot(x, y + size/2, size/2);
    }
    
    static void drawDash(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glEnd();
    }
    
    static void drawUnderscore(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawPlus(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size/2, y);
        glVertex2f(x + size/2, y + size);
        glEnd();
    }
    
    static void drawEquals(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size/3);
        glVertex2f(x + size, y + size/3);
        glVertex2f(x, y + 2*size/3);
        glVertex2f(x + size, y + 2*size/3);
        glEnd();
    }
    
    static void drawSlash(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawBackslash(float x, float y, float size) {
        glBegin(GL_LINES);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y);
        glEnd();
    }
    
    static void drawParenthesisLeft(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawParenthesisRight(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawBracketLeft(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawBracketRight(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y + size);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawAngleLeft(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x + size, y);
        glVertex2f(x, y + size/2);
        glVertex2f(x + size, y + size);
        glEnd();
    }
    
    static void drawAngleRight(float x, float y, float size) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x, y + size);
        glEnd();
    }
    
    static void drawUnknown(float x, float y, float size) {
        // Question mark in box for unknown characters
        drawO(x, y, size);
        drawQuestion(x, y, size);
    }
};

#endif