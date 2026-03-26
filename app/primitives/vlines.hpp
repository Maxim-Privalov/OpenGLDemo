#ifndef VLINES_H
#define VLINES_H
#define _USE_MATH_DEFINES

#include <GLFW/glfw3.h>
#include <optional>
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



struct Color {
    double r;
    double g;
    double b;
    double a;
    Color(double r, double g, double b, double a) : r{r}, g{g}, b{b}, a{a} {};
};

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
        plot(static_cast<int>(ypxl1), static_cast<int>(xpxl1), stroke_color, (1.0f - (yend - ypxl1)) * xgap);
        plot(static_cast<int>(ypxl1 + 1), static_cast<int>(xpxl1), stroke_color, (yend - ypxl1) * xgap);
    } else {
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1), stroke_color, (1.0f - (yend - ypxl1)) * xgap);
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1 + 1), stroke_color, (yend - ypxl1) * + xgap);
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
        plot(static_cast<int>(ypxl2), static_cast<int>(xpxl2), stroke_color, (1.0f - (yend - ypxl2)) * xgap);
        plot(static_cast<int>(ypxl2 + 1), static_cast<int>(xpxl2), stroke_color, (yend - ypxl2) * xgap);
    } else {
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2), stroke_color, (1.0f - (yend - ypxl2)) * xgap);
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2 + 1), stroke_color,(yend - ypxl2) * xgap);
    }

    glEnd();
}


// #### SutherlandCohen ####

int computeCode(Vertex v, int xmin, int xmax, int ymin, int ymax) {
    int code = 0;
    if (v.x < xmin) { code |= 1; }
    if (v.x > xmax) { code |= 2; }
    if (v.y < ymin) { code |= 4; }
    if (v.y > ymax) { code |= 8; }
    return code;
};
std::optional<std::pair<Vertex, Vertex>> sutherlandCohen(const Vertex &v1, const Vertex &v2, int xmin, int xmax, int ymin, int ymax) {
    int code1 = computeCode(v1, xmin, xmax, ymin, ymax);
    int code2 = computeCode(v2, xmin, xmax, ymin, ymax);

    Vertex v1_r = v1;
    Vertex v2_r = v2;
    double x = v1_r.x;
    double y = v1_r.y;

    bool accept = false;
    while (true) {
        if (code1 == 0 && code2 == 0) {
            accept = true;
            break;
        } else if ((code1 & code2) != 0) {
            break;
        } else {
            int codeOut = code1 != 0 ? code1 : code2;
            if (codeOut & 1) {
                x = xmin;
                y = v1_r.y + (v2_r.y - v1_r.y) * (xmin - v1_r.x) / (v2_r.x - v1_r.x);
            } else if (codeOut & 2) {
                x = xmax;
                y = v1_r.y + (v2_r.y - v1_r.y) * (xmax - v1_r.x) / (v2_r.x - v1_r.x);
            } else if (codeOut & 4) {
                y = ymin;
                x = v1_r.x + (v2_r.x - v1_r.x) * (ymin - v1_r.y) / (v2_r.y - v1_r.y);
            } else if (codeOut & 8) {
                y = ymax;
                x = v1_r.x + (v2_r.x - v1_r.x) * (ymax - v1_r.y) / (v2_r.y - v1_r.y);
            }
            if (codeOut == code1) {
                v1_r.x = x;
                v1_r.y = y;
                code1 = computeCode(Vertex(v1_r.x, v1_r.y), xmin, xmax, ymin, ymax);
            } else {
                v2_r.x = x;
                v2_r.y = y;
                code2 = computeCode(Vertex(v2_r.x, v2_r.y), xmin, xmax, ymin, ymax);
            }
        }
    }
    if (accept) {
        return std::make_pair(v1_r, v2_r);
    }
    return std::nullopt;
}


// #### LiangBarsky ####

std::optional<std::pair<Vertex, Vertex>> lineClip(const Vertex &v1, const Vertex &v2, int xmin, int xmax, int ymin, int ymax) {
    double dx = v2.x - v1.x;
    double dy = v2.y - v1.y;
    
    double t0 = 0.0;
    double t1 = 1.0;
    
    auto clipTest = [&](double p, double q) -> bool {
        if (p == 0) {
            return q >= 0;
        }
        
        double r = q / p;
        if (p < 0) {
            if (r > t0) t0 = r;
            if (t0 > t1) return false;
        } else {
            if (r < t1) t1 = r;
            if (t0 > t1) return false;
        }
        return true;
    };
    
    if (!clipTest(-dx, v1.x - xmin)) return std::nullopt;
    if (!clipTest( dx, xmax - v1.x)) return std::nullopt;
    if (!clipTest(-dy, v1.y - ymin)) return std::nullopt;
    if (!clipTest( dy, ymax - v1.y)) return std::nullopt;
    
    Vertex v1_r = v1;
    Vertex v2_r = v2;

    if (t0 > 0) {
        v1_r.x = v1.x + t0 * dx;
        v1_r.y = v1.y + t0 * dy;
    } else {
        v1_r.x = v1.x;
        v1_r.y = v1.y;
    }
    
    if (t1 < 1) {
        v2_r.x = v1.x + t1 * dx;
        v2_r.y = v1.y + t1 * dy;
    } else {
        v2_r.x = v2.x;
        v2_r.y = v2.y;
    }
    
    return std::make_pair(v1_r, v2_r);
}


void drawLineCrop(const Vertex &v1, const Vertex &v2, Color stroke_color, int xmin, int xmax, int ymin, int ymax) {
    auto vv = lineClip(v1, v2, xmin, xmax, ymin, ymax);
    if (vv.has_value()) {
        auto [v1, v2] = vv.value();
        drawLine(v1, v2, stroke_color, true);
    }
}



void drawArea(const std::vector<Vertex>& vertices, Color stroke_color, int xmin, int xmax, int ymin, int ymax) {
    for (std::size_t i = 0; i < vertices.size(); i++) {
        if (i + 1 == vertices.size()) {
            drawLineCrop(vertices[i], vertices[0], stroke_color, xmin, xmax, ymin, ymax);
        } else {
            drawLineCrop(vertices[i], vertices[i+1], stroke_color, xmin, xmax, ymin, ymax);
        }
    }
}


#endif
