#define _USE_MATH_DEFINES
#include <optional>
#include <GLFW/glfw3.h>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <utility>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace ublas = boost::numeric::ublas;

class Vertex {
    public:
        double x;
        double y;

        Vertex(double x, double y) : x{x}, y{y} {};
        Vertex(ublas::vector<double> ublas_vertex) : x{ublas_vertex(0)}, y{ublas_vertex(1)} {};
        ublas::vector<double> to_ublas() const {
            ublas::vector<double> vub(2);
            vub(0) = x; vub(1) = y;
            return vub;
        }
        friend std::ostream& operator<<(std::ostream& os, const Vertex& v) {
            return os << "(" << v.x << ", " << v.y << ")";
        }
};

struct Color {
    double r;
    double g;
    double b;
    double a;
    Color(double r, double g, double b, double a) : r{r}, g{g}, b{b}, a{a} {};
};

std::vector<Vertex> rotateVertex(const std::vector<Vertex>& vertices, const Vertex& center, double angle) {
    ublas::matrix<double> rM(2, 2);
    rM(0,0) = std::cos(angle); rM(0,1) = -std::sin(angle);
    rM(1,0) = std::sin(angle); rM(1,1) = std::cos(angle);
    std::vector<Vertex> rotated;

    for (const Vertex& v : vertices) {
        ublas::vector<double> u_v = v.to_ublas();
        u_v(0) -= center.x; u_v(1) -= center.y;
        ublas::vector<double> u_v_rotated = ublas::prod(rM, u_v);
        u_v_rotated(0) += center.x; u_v_rotated(1) += center.y;
        rotated.push_back(Vertex(u_v_rotated));
    }

    return rotated;

};

Vertex getCenterVertex(const Vertex& v1, const Vertex& v2) {
    return Vertex((v1.x + v2.x) / 2, (v1.y + v2.y) / 2);
}

static void plot(int x, int y, const Color& c, float brightness) {
    if (brightness <= 0.001f) return; 

    float alpha = brightness * static_cast<float>(c.a);
    if (alpha > 1.0f) alpha = 1.0f;

    glColor4f(static_cast<float>(c.r),
              static_cast<float>(c.g),
              static_cast<float>(c.b),
              alpha);
    glVertex2i(x, y);
}

void drawLine(const Vertex& v1, const Vertex& v2, Color stroke_color, bool anti_alias = true) {
    double x0 = v1.x;
    double y0 = v1.y;
    double x1 = v2.x;
    double y1 = v2.y;

    // Если выключен антиалиасинг — используем старый подход (округление + Bresenham)
    if (!anti_alias) {
        int ix0 = static_cast<int>(std::round(x0));
        int iy0 = static_cast<int>(std::round(y0));
        int ix1 = static_cast<int>(std::round(x1));
        int iy1 = static_cast<int>(std::round(y1));

        double dx = std::abs(ix1 - ix0);
        double dy = -std::abs(iy1 - iy0);
        double sx = ix0 < ix1 ? 1.0 : -1.0;
        double sy = iy0 < iy1 ? 1.0 : -1.0;
        double err = dx + dy;

        glBegin(GL_POINTS);
        while (true) {
            glColor4f(static_cast<float>(stroke_color.r),
                      static_cast<float>(stroke_color.g),
                      static_cast<float>(stroke_color.b),
                      static_cast<float>(stroke_color.a));
            glVertex2i(ix0, iy0);

            if (std::abs(ix0 - ix1) < 0.1 && std::abs(iy0 - iy1) < 0.1) break;

            double e2 = 2 * err;
            if (e2 >= dy) { err += dy; ix0 += static_cast<int>(sx); }
            if (e2 <= dx) { err += dx; iy0 += static_cast<int>(sy); }
        }
        glEnd();
        return;
    }
    // ────────────────────────────────────────────────
    // Xiaolin Wu's algorithm (антиалиасинг включён)
    // ────────────────────────────────────────────────
    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = (dx == 0.0) ? 1.0 : dy / dx;

    // Первый конец линии (с обработкой fractional части)
    double xend = std::round(x0);
    double yend = y0 + gradient * (xend - x0);
    double xgap = 1.0 - std::abs(x0 + 0.5 - xend);
    double xpxl1 = xend;
    double ypxl1 = std::floor(yend + 0.5);

    glBegin(GL_POINTS);

    if (steep) {
        plot(static_cast<int>(ypxl1), static_cast<int>(xpxl1), stroke_color, (1.0f - (yend - ypxl1)) * static_cast<float>(xgap));
        plot(static_cast<int>(ypxl1 + 1), static_cast<int>(xpxl1), stroke_color, (yend - ypxl1) * static_cast<float>(xgap));
    } else {
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1), stroke_color, (1.0f - (yend - ypxl1)) * static_cast<float>(xgap));
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1 + 1), stroke_color, (yend - ypxl1) * static_cast<float>(xgap));
    }

    double intery = yend + gradient;  // первая fractional y для основного цикла

    // Основной цикл
    for (double x = xpxl1 + 1.0; x <= std::round(x1) - 0.5; ++x) {
        double yf = std::floor(intery + 0.5);

        float c1 = static_cast<float>(1.0 - (intery - yf));
        float c2 = static_cast<float>(intery - yf);

        if (steep) {
            plot(static_cast<int>(yf), static_cast<int>(x), stroke_color, c1);
            plot(static_cast<int>(yf + 1), static_cast<int>(x), stroke_color, c2);
        } else {
            plot(static_cast<int>(x), static_cast<int>(yf), stroke_color, c1);
            plot(static_cast<int>(x), static_cast<int>(yf + 1), stroke_color, c2);
        }

        intery += gradient;
    }

    // Последний конец линии
    xend = std::round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = std::abs(x1 + 0.5 - xend);
    double xpxl2 = xend;
    double ypxl2 = std::floor(yend + 0.5);

    if (steep) {
        plot(static_cast<int>(ypxl2), static_cast<int>(xpxl2), stroke_color, (1.0f - (yend - ypxl2)) * static_cast<float>(xgap));
        plot(static_cast<int>(ypxl2 + 1), static_cast<int>(xpxl2), stroke_color, (yend - ypxl2) * static_cast<float>(xgap));
    } else {
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2), stroke_color, (1.0f - (yend - ypxl2)) * static_cast<float>(xgap));
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2 + 1), stroke_color,(yend - ypxl2) * static_cast<float>(xgap));
    }

    glEnd();
}

void drawArea(const std::vector<Vertex>& vertices, Color stroke_color) {
    for (std::size_t i = 0; i < vertices.size(); i++) {
        if (i + 1 == vertices.size()) {
            drawLine(vertices[i], vertices[0], stroke_color, true);
        } else {
            drawLine(vertices[i], vertices[i+1], stroke_color, true);
        }
    }
}



class Primitive {
    private:
        std::vector<Vertex> vertices;
        Color stroke_color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    protected:
        virtual void rotate(double angle, std::optional<Vertex> center = std::nullopt) = 0;

        void setVertices(const std::vector<Vertex>& v) {
            vertices = v;
        }
    
    public:
        std::vector<Vertex> getVertices() const {
            return vertices;
        }

        void strokeColor(Color color) {
            stroke_color = color;
        }
        
        void render() const {
            drawArea(vertices, stroke_color);
        }

};

class Rect : public Primitive {
    private:
        Vertex v1;
        Vertex v2;

    public:
        Rect(const Vertex& v1, const Vertex& v2) : v1{v1}, v2{v2} {
            std::vector<Vertex> vv = {v1, Vertex(v2.x, v1.y), v2, Vertex(v1.x, v2.y)};
            setVertices(std::move(vv));
        }

        Rect(const Vertex& pos, int width, int height) : Rect(pos, Vertex(pos.x + width, pos.y + height)) {}

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                center = getCenterVertex(v1, v2);
            }
            setVertices(rotateVertex(getVertices(), *center, angle));
        }
};

class Triangle : public Primitive {

    public:
        Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
            std::vector<Vertex> vv = {v1, v2, v3};
            setVertices(std::move(vv));
        }

        Triangle(const Vertex& start_pos, const Vertex& rel_v1, const Vertex& rel_v2, const Vertex& rel_v3) :
            Triangle(
                    Vertex(start_pos.x + rel_v1.x, start_pos.y + rel_v1.y), 
                    Vertex(start_pos.x + rel_v2.x, start_pos.y + rel_v2.y), 
                    Vertex(start_pos.x + rel_v3.x, start_pos.y + rel_v3.y)
            ) {}

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                std::vector<Vertex> v = getVertices();
                center = {(v[0].x + v[1].x + v[2].x) / 3, (v[0].y + v[1].y + v[2].y) / 3};
            }
            setVertices(rotateVertex(getVertices(), *center, angle));
        }
};