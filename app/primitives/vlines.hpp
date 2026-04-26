#ifndef VLINES_H
#define VLINES_H
#define _USE_MATH_DEFINES

#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <unordered_map>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <utility>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace ublas = boost::numeric::ublas;

// Отнаследуемся от tuple чтобы обеспечить структурное связывание (распаковку значений полей в переменную), типа: double [x, y] = vert
class Vertex : public std::tuple<int, double, std::string> {
    public:
        using std::tuple<int, double, std::string>::tuple;

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
        // возвращаем как кортеж для распаковки
        auto as_tuple() {
            return std::make_tuple(x, y);
        }
        auto as_tuple() const {
            return std::tie(x, y);
        }
};


/*
Есть 2 способа отрезать кусок фигуры, на разных уровнях: 
- Уровень контрольных точек -> отрезание или корректировка точек полигона до отрисовки (для фигур сделанных из ломанных)
 (drawClipLine в тривиальном случае, polygonClip, fillArea no clipping уже по готовым точкам)

- Уровень отрисовки -> отрезание "грубое", точки не корректируются, а выборочно рисуются (для гладких фигур и кривых)
 (drawClipLine в нетривиальном случае, fillArea with clipping (scan line с обрезкой))
*/
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

std::vector<Vertex> parallelMove(const std::vector<Vertex>& vertices, const Vertex& spos, const Vertex& epos) {
    std::vector<Vertex> movedVertices;
    movedVertices.reserve(vertices.size());
    auto vectord = Vertex(epos.x - spos.x, epos.y - spos.y);
    for (const auto& v : vertices) {
        movedVertices.push_back(Vertex(v.x + vectord.x, v.y + vectord.y));
    }
    return movedVertices;
};

Vertex getCenterVertex(const Vertex& v1, const Vertex& v2) {
    return Vertex((v1.x + v2.x) / 2, (v1.y + v2.y) / 2);
}

struct Color {
    double r;
    double g;
    double b;
    double a;
    Color(int r, int g, int b, double a) : r{static_cast<double>(r)/255}, g{static_cast<double>(g)/255}, b{static_cast<double>(b)/255}, a{a} {};
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

void drawLine(const Vertex& v1, const Vertex& v2, Color stroke_color, int xmin, int xmax, int ymin, int ymax, bool anti_alias) {
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
        plot(static_cast<int>(ypxl1), static_cast<int>(xpxl1), stroke_color, static_cast<float>((1.0f - (yend - ypxl1)) * xgap));
        plot(static_cast<int>(ypxl1 + 1), static_cast<int>(xpxl1), stroke_color, static_cast<float>((yend - ypxl1) * xgap));
    } else {
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1), stroke_color, static_cast<float>((1.0f - (yend - ypxl1)) * xgap));
        plot(static_cast<int>(xpxl1), static_cast<int>(ypxl1 + 1), stroke_color, static_cast<float>((yend - ypxl1) * + xgap));
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
        plot(static_cast<int>(ypxl2), static_cast<int>(xpxl2), stroke_color, static_cast<float>((1.0f - (yend - ypxl2)) * xgap));
        plot(static_cast<int>(ypxl2 + 1), static_cast<int>(xpxl2), stroke_color, static_cast<float>((yend - ypxl2) * xgap));
    } else {
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2), stroke_color, static_cast<float>((1.0f - (yend - ypxl2)) * xgap));
        plot(static_cast<int>(xpxl2), static_cast<int>(ypxl2 + 1), stroke_color, static_cast<float>((yend - ypxl2) * xgap));
    }

    glEnd();
}

std::optional<std::pair<Vertex, Vertex>> lineClip(const Vertex &v1, const Vertex &v2, int xmin, int xmax, int ymin, int ymax) {
    double dx = v2.x - v1.x;
    double dy = v2.y - v1.y;
    
    // Обработка вырожденного случая - точка
    if (dx == 0 && dy == 0) {
        if (v1.x >= xmin && v1.x <= xmax && v1.y >= ymin && v1.y <= ymax) {
            return std::make_pair(v1, v1);
        }
        return std::nullopt;
    }
    
    double t0 = 0.0;
    double t1 = 1.0;
    
    auto clipTest = [&](double p, double q) -> bool {
        if (p == 0) {
            return q >= 0;  // Параллельно границе
        }
        
        double r = q / p;
        if (p < 0) {
            if (r > t0) t0 = r;
        } else {
            if (r < t1) t1 = r;
        }
        return t0 <= t1;
    };
    
    // Проверка всех 4 границ
    if (!clipTest(-dx, v1.x - xmin)) return std::nullopt;
    if (!clipTest( dx, xmax - v1.x)) return std::nullopt;
    if (!clipTest(-dy, v1.y - ymin)) return std::nullopt;
    if (!clipTest( dy, ymax - v1.y)) return std::nullopt;
    
    // Вычисление обрезанных вершин
    Vertex v1_r, v2_r;
    
    if (t1 < 1) {
        v2_r.x = v1.x + t1 * dx;
        v2_r.y = v1.y + t1 * dy;
    } else {
        v2_r = v2;
    }
    
    if (t0 > 0) {
        v1_r.x = v1.x + t0 * dx;
        v1_r.y = v1.y + t0 * dy;
    } else {
        v1_r = v1;
    }
    
    return std::make_pair(v1_r, v2_r);
}

// liam-barsky
void drawClipLine(const Vertex& v1, const Vertex& v2, Color stroke_color, int xmin, int xmax, int ymin, int ymax, bool anti_alias) {
    auto vv = lineClip(v1, v2, xmin, xmax, ymin, ymax);
    if (vv.has_value()) {
        drawLine(std::get<0>(vv.value()), std::get<1>(vv.value()), stroke_color, xmin, xmax, ymin, ymax, anti_alias);
    }
}

// Сазерленда-Ходжмана
std::vector<Vertex> polygonClip(const std::vector<Vertex> &polygon, int xmin, int xmax, int ymin, int ymax) {
    std::vector<Vertex> result = polygon;
    
    auto clipEdge = [](const std::vector<Vertex>& input, 
                       int coord, bool isMin, int bound) -> std::vector<Vertex> {
        std::vector<Vertex> output;
        if (input.empty()) return output;
        
        for (size_t i = 0; i < input.size(); i++) {
            const Vertex& current = input[i];
            const Vertex& next = input[(i + 1) % input.size()];
            
            // Проверка, видима ли точка
            bool currentInside = isMin ? 
                (coord == 0 ? current.x >= bound : current.y >= bound) :
                (coord == 0 ? current.x <= bound : current.y <= bound);
            
            bool nextInside = isMin ? 
                (coord == 0 ? next.x >= bound : next.y >= bound) :
                (coord == 0 ? next.x <= bound : next.y <= bound);
            
            // Случай 1: переход изнутри наружу
            if (currentInside && !nextInside) {
                // Найти точку пересечения
                double t;
                if (coord == 0) {
                    t = (bound - current.x) / (next.x - current.x);
                } else {
                    t = (bound - current.y) / (next.y - current.y);
                }
                Vertex intersect;
                intersect.x = current.x + t * (next.x - current.x);
                intersect.y = current.y + t * (next.y - current.y);
                output.push_back(intersect);
            }
            // Случай 2: переход изнаружи внутрь
            else if (!currentInside && nextInside) {
                double t;
                if (coord == 0) {
                    t = (bound - current.x) / (next.x - current.x);
                } else {
                    t = (bound - current.y) / (next.y - current.y);
                }
                Vertex intersect;
                intersect.x = current.x + t * (next.x - current.x);
                intersect.y = current.y + t * (next.y - current.y);
                output.push_back(intersect);
                output.push_back(next);
            }
            // Случай 3: обе внутри
            else if (currentInside && nextInside) {
                output.push_back(next);
            }
            // Случай 4: обе снаружи - ничего не добавляем
        }
        return output;
    };
    
    // Последовательное отсечение по 4 границам
    result = clipEdge(result, 0, true, xmin);   // левая
    result = clipEdge(result, 0, false, xmax);  // правая
    result = clipEdge(result, 1, true, ymin);   // нижняя
    result = clipEdge(result, 1, false, ymax);  // верхняя
    
    return result;
}

// scanline
void fillArea(const std::vector<Vertex>& vertices, Color fill_color, bool with_clipping = false, int xMin = 0, int xMax = 0, int yMin = 0, int yMax = 0) {
    if (vertices.size() < 3) return;

    double ymin = vertices[0].y;
    double ymax = vertices[0].y;

    for (const auto& v : vertices) {
        ymin = std::min(ymin, v.y);
        ymax = std::max(ymax, v.y);
    }

    int y_start = (int)std::ceil(ymin);
    int y_end   = (int)std::floor(ymax);

    for (int y = y_start; y <= y_end; ++y) {
        std::vector<double> intersections;

        for (size_t i = 0; i < vertices.size(); ++i) {
            auto v1 = vertices[i];
            auto v2 = vertices[(i + 1) % vertices.size()];

            double x1 = v1.x;
            double y1 = v1.y;
            double x2 = v2.x;
            double y2 = v2.y;

            // пропускаем горизонтальные рёбра
            if (std::abs(y1 - y2) < 1e-6)
                continue;

            // делаем y1 <= y2
            if (y1 > y2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }

            if (y < std::ceil(y1) || y >= std::ceil(y2))
                continue;

            // пересечение с горизонтальной линией
            double t = (y - y1) / (y2 - y1);
            double x = x1 + t * (x2 - x1);

            intersections.push_back(x);
        }

        if (intersections.size() < 2)
            continue;

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            int x_start = (int)std::ceil(intersections[i]);
            int x_end   = (int)std::floor(intersections[i + 1]);

            if (x_start <= x_end) {
                if (!with_clipping) {
                    drawLine(
                        Vertex(x_start, y),
                        Vertex(x_end, y),
                        fill_color,
                        0, 0, 0, 0, false
                    );
                } else {
                    drawClipLine(Vertex(x_start, y),
                        Vertex(x_end, y),
                        fill_color,
                        xMin, xMax, yMin, yMax, false);
                }
            }
        }
    }
}

void drawArea(std::vector<Vertex> vertices,
              Color stroke_color,
              std::optional<Color> fill_color,
              int xmin, int xmax, 
              int ymin, int ymax,
              bool restruct_border = false) {
    
    if (vertices.size() > 2) {
        auto cropped = polygonClip(vertices, xmin, xmax, ymin, ymax);
        if (fill_color.has_value()) {
            fillArea(cropped, fill_color.value());
        }
        void (*drawNewLine)(const Vertex &v1, const Vertex &v2, Color stroke_color, int xmin, int xmax, int ymin, int ymax, bool anti_alias) = drawLine;
        if (!restruct_border) {
            // Для лабы 2 задание 3 --- (drawNewLine = drawLine)
            drawNewLine = drawLine;
            cropped = vertices;
        }
        
        for (std::size_t i = 0; i < cropped.size(); i++) {
            std::size_t next = i + 1;
            if (i + 1 == cropped.size()) {
                next = 0;
            }
            drawNewLine(cropped[i], cropped[next], stroke_color, xmin, xmax, ymin, ymax, true);
        }
    }
}


#endif
