#ifndef POLIGONS_H
#define POLIGONS_H
#define _USE_MATH_DEFINES

#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include "../primitives/vlines.hpp"
#include "../primitives/2dshapes.hpp"


std::vector<Vertex> getRdVertexOnEllipsis(const Ellipsis& ell, std::size_t counts, std::mt19937 gen) {
    std::vector<Vertex> result;
    result.reserve(counts);
    std::uniform_real_distribution<> points_dist_x(0, 2*M_PI);
    auto [a, b] = ell.getSizePair(); a = a / 2; b = b / 2;
    std::vector<double> TT;
    TT.reserve(counts);
    for (std::size_t i = 0; i < counts; i++) {  
        TT.push_back(points_dist_x(gen));
    }
    std::sort(TT.begin(), TT.end()); 
    for (const auto& t : TT) {
        result.push_back(Vertex(a*std::cos(t), b*std::sin(t)));
    }
    return parallelMove(result, Vertex(0, 0), ell.getPosition());
};


std::vector<Vertex> getRdVertexStarlike(const Ellipsis& ell, std::size_t counts, std::mt19937 gen) {
    std::vector<Vertex> result;
    result.reserve(2*counts);
    std::uniform_real_distribution<> points_dist_x(0, 2*M_PI);
    auto [a, b] = ell.getSizePair(); a = a / 2; b = b / 2;
    std::vector<double> TT;
    TT.reserve(counts);
    for (std::size_t i = 0; i < counts; i++) {  
        TT.push_back(points_dist_x(gen));
    }
    std::sort(TT.begin(), TT.end()); 
    for (const auto& t : TT) {
        result.push_back(Vertex(a*std::cos(t), b*std::sin(t)));
        result.push_back(Vertex((a/2)*std::cos(t + M_PI/12), (b/2)*std::sin(t + M_PI/12)));
    }
    return parallelMove(result, Vertex(0, 0), ell.getPosition());
};

std::vector<Vertex> getButterflyPolygon(const Rect& section_rectangle, std::mt19937 gen) {
    auto [w, h] = section_rectangle.getSizePair();
    auto [xPos, yPos] = section_rectangle.getVertices()[0].as_tuple();

    std::vector<Vertex> vertices;

    for (std::size_t i = 0; i < 2; i++) {
        auto x_dist = std::uniform_real_distribution<>(xPos + (i*w/2), xPos + w/2 + (i*w/2));

        for (std::size_t j = 0; j < 2; j++) {
            auto y_dist = std::uniform_real_distribution<>(yPos + (j*h/2), yPos + h/2 + (j*h/2));
            vertices.push_back(Vertex(x_dist(gen), y_dist(gen)));
        }
    }
    return vertices;
}

std::vector<Vertex> getSpiralPolygon(const Rect& section_rectangle, double endPhi, double step, std::mt19937 gen) {
    auto [w, h] = section_rectangle.getSizePair();
    auto [xPos, yPos] = section_rectangle.getVertices()[0].as_tuple();

    auto start_pos = Vertex(xPos + w/2, yPos + h/2);
    double startPhi = 0;

    double P = (1 + sqrt(5))/2;
    P = 1.1;
    
    double x = 0;
    double y = 0;
    double R = 10;
    std::vector<Vertex> result;
    while (startPhi < endPhi) {
        x = R * cos(startPhi);
        y = R * sin(startPhi);
        R = R * P;
        result.push_back(Vertex(x, y));
        startPhi += step;
    }

    return parallelMove(result, Vertex(0, 0), start_pos);;
}




#endif