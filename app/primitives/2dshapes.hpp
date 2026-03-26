#ifndef TWO_D_SHAPES_H
#define TWO_D_SHAPES_H

#include "vlines.hpp"
#include <vector>
#include <utility>


class Primitive {
    private:
        std::vector<Vertex> vertices;
        Color stroke_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        std::pair<Vertex, Vertex> crop_border = {Vertex(0, 0), Vertex(100000, 100000)};

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

        void setCropBorder(const Vertex &v1, const Vertex &v2) {
            crop_border = std::make_pair(v1, v2);
        }
        
        void render() const {
            Vertex v0 = crop_border.first;
            Vertex v1 = crop_border.second;
            drawArea(vertices, stroke_color, v0.x, v1.x, v0.y, v1.y);
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

#endif