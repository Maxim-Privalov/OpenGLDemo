#ifndef POLIGONS_H
#define POLIGONS_H
#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <boost/circular_buffer.hpp>

#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include <stdexcept>

#include "vlines.hpp"
#include "2dshapes.hpp"


template <typename T>
class ringed_vector : public std::vector<T> {
    public:
        using std::vector<T>::vector;

        ringed_vector(std::vector<T> vect) {
            for (auto& v : vect) {
                this->push_back(v);
            }
        }

        T& at(int i) {
            if (this->empty()) {
                throw std::out_of_range("ringed vector -> index out of range");
            }
            if (i < 0) {
                i = static_cast<int>(this->size()) + i;
            }
            i = signum(i)*(i % static_cast<int>(this->size()));
            return std::vector<T>::at(i);
        }

};

auto signum = [](int val) -> int {
    return val == 0 ? 0 : (val > 0 ? 1 : -1);
};

auto cross = [](const Vertex& A, const Vertex& B, const Vertex& C) -> double {
    return (B.x - A.x)*(C.y - B.y) - (B.y - A.y)*(C.x - B.x);
};

auto is_inside_triangle = [](const Vertex& A, const Vertex& B, const Vertex& C, const Vertex& P) -> bool {
    double c1 = cross(A, B, P);
    double c2 = cross(B, C, P);
    double c3 = cross(C, A, P);
                        
    return ((c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0));
};

bool is_convex(const std::vector<Vertex>& vertices) {
    double expected_cross = 0;
    for (int i = 0; i < vertices.size(); i++) {
        int A = (i == 0) ? static_cast<int>(vertices.size()) - 1 : i - 1;
        int C = (i == vertices.size() - 1) ? 0 : i + 1;
        double tr_cross = cross(vertices[A], vertices[i], vertices[C]);
        if (expected_cross == 0) {
            expected_cross = tr_cross;
        }
        if (!((tr_cross == 0) || (expected_cross > 0 && tr_cross > 0) || (expected_cross < 0 && tr_cross < 0))) {
            return false;
        }
    }
    return true;
};

std::vector<Triangle> triangulate(const std::vector<Vertex>& vertices) {
    std::vector<Triangle> triangles;

    if (is_convex(vertices)) {
        for (std::size_t i = 1; i < vertices.size(); i++) {
            triangles.push_back({vertices[0], vertices[i - 1], vertices[i]});
        }
    } else {
        auto rvertices = static_cast<ringed_vector<Vertex>>(vertices);

        double Sorient = 0;
        for (int i = 0; i < rvertices.size(); i++) {
            Sorient = Sorient + rvertices.at(i).x * rvertices.at(i+1).y - rvertices.at(i+1).x * rvertices.at(i).y;
        }
        if (Sorient != 0) {
            int i = 0;
            while (rvertices.size() > 3) {
                double tr_cross = cross(rvertices.at(i-1), rvertices.at(i), rvertices.at(i+1));
                if ((Sorient > 0 && tr_cross > 0) || (Sorient < 0 && tr_cross < 0)) {
                    bool is_have_inside = false;
                    for (int j = 0; j < rvertices.size(); j++) {
                        if (j <= i + 1 && j >= i - 1) {
                            continue;
                        }
                        if (is_inside_triangle(rvertices.at(i - 1), rvertices.at(i), rvertices.at(i + 1), rvertices.at(j))) {
                            is_have_inside = true;
                            break;
                        }
                    }
                    if (!is_have_inside) {
                        triangles.push_back({rvertices.at(i - 1), rvertices.at(i), rvertices.at(i + 1)});
                        rvertices.erase(rvertices.begin() + i);

                    }
                }
                i = (i == rvertices.size() - 1) ? 0 : i + 1;
            }
            triangles.push_back({rvertices.at(-1), rvertices.at(1), rvertices.at(0)});
        }
    }
    
    return triangles;
}


std::vector<Triangle> triangulate(const std::vector<Vertex>& vertices, const std::vector<Vertex>& inner_vertices) {
    std::vector<Triangle> triangles;
    for (const auto& i_v : inner_vertices) {
        bool point_in_polygon = false;
        for (const auto& tr : triangulate(vertices)) {
            auto tr_vert = tr.getVertices();
            if (!is_inside_triangle(tr_vert[0], tr_vert[1], tr_vert[2], i_v)) {
                point_in_polygon = true;
                break;
            }
        }
        if (!point_in_polygon) {
            throw std::invalid_argument("There is a vertex that extends beyond the boundaries of the triangulation polygon");
        }
    }

    std::pair<std::vector<Vertex>, std::vector<Vertex>> pair_vertices;
    ringed_vector rinner_vert = inner_vertices;
    for (const auto& v : vertices) {
        pair_vertices.first.push_back(v);
    }
    for (int i = 1; i <= rinner_vert.size(); i++) {
        pair_vertices.first.push_back(rinner_vert.at((-1) * i));
    }
    pair_vertices.second = {vertices[0], vertices[vertices.size() - 1], rinner_vert.at(-1), rinner_vert.at(0)};
    
    std::vector<Triangle> second_triangles = triangulate(pair_vertices.first);
    triangles = triangulate(pair_vertices.second);
    triangles.insert(triangles.end(), second_triangles.begin(), second_triangles.end());
    return triangles;
}

bool in_polygon(const std::vector<Vertex>& vertices, const Vertex& check_vertex) {
    for (const auto& tr : triangulate(vertices)) {
        auto tr_vert = tr.getVertices();
        if (is_inside_triangle(tr_vert[0], tr_vert[1], tr_vert[2], check_vertex)) {
            return true; 
        }
    }
    return false;
}


#endif