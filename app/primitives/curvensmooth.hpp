#ifndef CURVENSMOOTH_H
#define CURVENSMOOTH_H
#define _USE_MATH_DEFINES

#include <optional>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "vlines.hpp"


class Octant {
    public:
        Vertex P0;
        Vertex P1;
        Vertex P2;
        Vertex P3;

        Octant(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4) : P0{v1}, P1{v2}, P2{v3}, P3{v4} {}
        Octant(const std::vector<Vertex>& v) : P0{v[0]}, P1{v[1]}, P2{v[2]}, P3{v[3]} {}

        std::vector<Vertex> to_vector() const {
            return {P0, P1, P2, P3};
        }

        friend std::ostream& operator<<(std::ostream& os, const Octant& o) {
            return os << "(" << o.P0 << ", " << o.P1 << ", " << o.P2 << ", " << o.P3 << ")";
        }
};

Vertex mid(const Vertex& v1, const Vertex& v2) {
    return { (v1.x + v2.x) * 0.5, (v1.y + v2.y) * 0.5 };
}

bool isFlat(const Octant& octant) {
    // Манхэттенское расстояние
    int ux = static_cast<int>(3 * octant.P1.x - 2 * octant.P0.x - octant.P3.x); ux *= ux;
    int uy = static_cast<int>(3 * octant.P1.y - 2 * octant.P0.y - octant.P3.y); uy *= uy;
    int vx = static_cast<int>(3 * octant.P2.x - octant.P0.x - 2 * octant.P3.x); vx *= vx;
    int vy = static_cast<int>(3 * octant.P2.y - octant.P0.y - 2 * octant.P3.y); vy *= vy;

    return (ux + uy < 16) && (vx + vy < 16);
}

void drawBezierRecursive(const Octant& octant, Color stroke_color, int xmin, int xmax, int ymin, int ymax, bool anti_alias = true, int limit = 12) {
    // Де-Кастельжо
    if (limit == 0 || isFlat(octant)) {
        drawClipLine(octant.P0, octant.P3, stroke_color, xmin, xmax, ymin, ymax, anti_alias);
        return;
    }

    Vertex P01 = mid(octant.P0, octant.P1);
    Vertex P12 = mid(octant.P1, octant.P2);
    Vertex P23 = mid(octant.P2, octant.P3);
    Vertex P012 = mid(P01, P12);
    Vertex P123 = mid(P12, P23);
    Vertex P0123 = mid(P012, P123); // Точка разделения

    Octant left(octant.P0, P01, P012, P0123);
    Octant right(P0123, P123, P23, octant.P3);
    // Левая и правая половина
    drawBezierRecursive(left, stroke_color, xmin, xmax, ymin, ymax, anti_alias, limit - 1);
    drawBezierRecursive(right, stroke_color, xmin, xmax, ymin, ymax, anti_alias, limit - 1);
}

void drawEllipsis(const Octant& octant, Color stroke_color, int xmin, int xmax, int ymin, int ymax, bool anti_alias = true) {
    drawBezierRecursive(octant, stroke_color, xmin, xmax, ymin, ymax, anti_alias);
    drawBezierRecursive(Octant(octant.P0, 
                            Vertex(octant.P0.x + (octant.P0.x - octant.P1.x), octant.P0.y + (octant.P0.y - octant.P1.y)), 
                            Vertex(octant.P3.x + (octant.P3.x - octant.P2.x), octant.P3.y + (octant.P3.y - octant.P2.y)), 
                            octant.P3), stroke_color, xmin, xmax, ymin, ymax, anti_alias);
}

auto get_ellipsis_approx_segment = [](int ellipsis_height, int ellipsis_width) -> int {
    float mid = static_cast<float>(ellipsis_height + ellipsis_width) / 2;
    if (mid < 10) {
        return static_cast<int>(0.5 * mid);
    } else if (mid >= 10 && mid < 50) {
        return static_cast<int>(0.25 * (mid + 10));
    } else if (mid >= 50 && mid < 280) {
        return static_cast<int>(0.10 * (mid + 150));
    } else {
        return 65;
    }
};

std::vector<Vertex> bezier_approximation(const Octant& octant, int segments = 32) {
    std::vector<Vertex> points;
    points.reserve(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        double mt = 1.0f - t;
        
        double mt2 = mt * mt;
        double t2 = t * t;

        // Коэффициенты для кубической кривой
        double b0 = mt2 * mt;
        double b1 = 3 * mt2 * t;
        double b2 = 3 * mt * t2;
        double b3 = t2 * t;

        double x = b0 * octant.P0.x + b1 * octant.P1.x + b2 * octant.P2.x + b3 * octant.P3.x;
        double y = b0 * octant.P0.y + b1 * octant.P1.y + b2 * octant.P2.y + b3 * octant.P3.y;
        points.push_back(Vertex(x, y));
    }

    return points;
}

void fillEllipsis(const Octant& octant, Color fill_color, int xmin, int xmax, int ymin, int ymax, int segments = 32) {
    fillArea(bezier_approximation(octant, segments), fill_color, true, xmin, xmax, ymin, ymax);
    fillArea(bezier_approximation(Octant(octant.P0, 
                            Vertex(octant.P0.x + (octant.P0.x - octant.P1.x), octant.P0.y + (octant.P0.y - octant.P1.y)), 
                            Vertex(octant.P3.x + (octant.P3.x - octant.P2.x), octant.P3.y + (octant.P3.y - octant.P2.y)), 
                            octant.P3), segments), fill_color, true, xmin, xmax, ymin, ymax);
}

#endif