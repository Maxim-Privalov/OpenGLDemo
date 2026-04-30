#ifndef TWO_D_SHAPES_H
#define TWO_D_SHAPES_H

#include "vlines.hpp"
#include "curvensmooth.hpp"
#include <vector>
#include <utility>

class Primitive {
    protected:
        std::vector<Vertex> vertices;
        Color stroke_color = Color(255, 255, 255, 1);
        std::optional<Color> fill_color = std::nullopt;
        std::pair<Vertex, Vertex> crop_border = {Vertex(0, 0), Vertex(100000, 100000)};
        bool restruct_border = false;

    public:
        virtual void rotate(double angle, std::optional<Vertex> center = std::nullopt) = 0;

        virtual void move(const Vertex &spos, const Vertex &epos) {
            vertices = parallelMove(vertices, spos, epos);
        }

        const std::vector<Vertex> &getVertices() const {
            return vertices;
        }

        void strokeColor(Color color) {
            stroke_color = color;
        }

        void fillColor(Color color) {
            fill_color = color;
        }

        void clearFillColor() {
            fill_color = std::nullopt;
        }

        void setCropBorder(const Vertex &v1, const Vertex &v2, bool restruct_border = false) {
            crop_border = std::make_pair(v1, v2);
            this->restruct_border = restruct_border;
        }

        void clearCropBorder() {
            crop_border = {Vertex(0, 0), Vertex(100000, 100000)};
            this->restruct_border = false;
        }

        virtual void render() const {
            Vertex v0 = crop_border.first;
            Vertex v1 = crop_border.second;
            if (vertices.size() > 2) {
                drawArea(vertices, stroke_color, fill_color,
                        static_cast<int>(v0.x),
                        static_cast<int>(v1.x),
                        static_cast<int>(v0.y),
                        static_cast<int>(v1.y),
                        restruct_border);
            }
            else {
                drawClipLine(vertices[0], vertices[1], stroke_color,
                            static_cast<int>(v0.x),
                            static_cast<int>(v1.x),
                            static_cast<int>(v0.y),
                            static_cast<int>(v1.y), true);
            }
        }
    };

class Line : public Primitive {
    public:
        Line(const Vertex &v1, const Vertex &v2) {
            vertices = {v1, v2};
        }
        std::tuple<Vertex, Vertex> getVerticesAsTuple() const {
            return std::make_tuple(vertices[0], vertices[1]);
        }

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                auto [v1, v2] = getVerticesAsTuple();
                center = getCenterVertex(v1, v2);
            }
            vertices = rotateVertex(vertices, center.value(), angle);
        }
};

class Rect : public Primitive {
    private:
        Vertex v1;
        Vertex v2;

    public:
        Rect(const Vertex &v1, const Vertex &v2) : v1{v1}, v2{v2} {
            vertices = {v1, Vertex(v2.x, v1.y), v2, Vertex(v1.x, v2.y)};
        }

        Rect(const Vertex &pos, int width, int height) : Rect(pos, Vertex(pos.x + width, pos.y + height)) {}

        std::pair<int, int> getSizePair() const {
            return std::make_pair(
                static_cast<int>(std::round(sqrt(
                    std::pow(abs(vertices[1].x - vertices[0].x), 2) + std::pow(abs(vertices[1].y - vertices[0].y), 2)))),
                static_cast<int>(std::round(sqrt(
                    std::pow(abs(vertices[1].x - vertices[2].x), 2) + std::pow(abs(vertices[1].y - vertices[2].y), 2)))
                ));
        }

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                center = getCenterVertex(v1, v2);
            }
            vertices = rotateVertex(vertices, center.value(), angle);
        }
};

class Triangle : public Primitive {

    public:
        Triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
            vertices = {v1, v2, v3};
        }

        Triangle(const Vertex &start_pos, const Vertex &rel_v1, const Vertex &rel_v2, const Vertex &rel_v3) : 
                    Triangle(
                            Vertex(start_pos.x + rel_v1.x, start_pos.y + rel_v1.y),
                            Vertex(start_pos.x + rel_v2.x, start_pos.y + rel_v2.y),
                            Vertex(start_pos.x + rel_v3.x, start_pos.y + rel_v3.y)) {}

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                center = {(vertices[0].x + vertices[1].x + vertices[2].x) / 3, (vertices[0].y + vertices[1].y + vertices[2].y) / 3};
            }
            vertices = rotateVertex(vertices, center.value(), angle);
        }
};

class Ellipsis : public Primitive {

    private:
        Octant octant;
        Vertex position;
        int width = 0;
        int height = 0;

    public:
        Ellipsis(const Vertex &pos, int width, int height) : position{pos}, width{width}, height{height},
                            octant(
                                Vertex(pos.x - (width / 2), pos.y),
                                Vertex(pos.x - (width / 2), pos.y - (height / 2) * 1.3333),
                                Vertex(pos.x + (width / 2), pos.y - (height / 2) * 1.3333),
                                Vertex(pos.x + (width / 2), pos.y)) {}

        Ellipsis(const Octant &octant) : octant{octant},
                            width{static_cast<int>(abs(octant.P3.x - octant.P0.x))},
                            height{static_cast<int>((abs(octant.P0.y - octant.P1.y) * 2) / 1.3333)},
                            position{Vertex(octant.P0.x + (octant.P3.x - octant.P0.x) / 2, octant.P0.y + (octant.P3.y - octant.P0.y) / 2)} {};

        void rotate(double angle, std::optional<Vertex> center = std::nullopt) override {
            if (!center.has_value()) {
                center = position;
            }
            octant = rotateVertex(octant.to_vector(), center.value(), angle);
        }

        void move(const Vertex &spos, const Vertex &epos) override {
            octant = parallelMove(octant.to_vector(), spos, epos);
        }

        const Octant &getOctant() const {
            return octant;
        }

        const Vertex &getPosition() const {
            return position;
        }

        std::pair<int, int> getSizePair() const {
            return std::make_pair(width, height);
        }

        void setControlPoints(const Vertex &P1, const Vertex &P2) {
            octant.P1 = P1;
            octant.P2 = P2;
        }

        void render() const override {
            Vertex v0 = crop_border.first;
            Vertex v1 = crop_border.second;
            if (fill_color.has_value()) {
                int segments_count = get_ellipsis_approx_segment(width, height);
                fillEllipsis(octant, fill_color.value(),
                            static_cast<int>(v0.x),
                            static_cast<int>(v1.x),
                            static_cast<int>(v0.y),
                            static_cast<int>(v1.y), segments_count);
            }
            drawEllipsis(octant, stroke_color,
                        static_cast<int>(v0.x),
                        static_cast<int>(v1.x),
                        static_cast<int>(v0.y),
                        static_cast<int>(v1.y), true);
        }
};

#endif