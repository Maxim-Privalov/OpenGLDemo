#ifndef FRACTALS_H
#define FRACTALS
#define _USE_MATH_DEFINES

#include <vector>
#include <utility>
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <stack>
#include <map>
#include "../primitives/2dshapes.hpp"


class Fractal {

    private:
        Vertex position = {0, 0};
        int rate = 1;
        double angle = 0;


        void recursive_branch_AD(Vertex pos, int length, double angle, double k, bool isleft, int rate, int i, 
        std::optional<std::uniform_real_distribution<>> angles_dist = std::nullopt,
        std::optional<std::mt19937> gen = std::nullopt) {

            std::pair<double, double> angles = {M_PI/4, M_PI/4};
            if (angles_dist.has_value() && gen.has_value()) {
                angles = {angles_dist.value()(gen.value()), angles_dist.value()(gen.value())};
            }

            std::vector branches = {Line(pos, Vertex(pos.x, pos.y + length)), Line(pos, Vertex(pos.x + length, pos.y))};
            int new_length = static_cast<int>(round(k * length));
            double new_angle = angle + (isleft ? angles.first : -angles.second);

            int left_right = 0;
            for (auto& br : branches) {
                br.strokeColor(isleft ? Color(255, 255, 255, 1) : Color(255, 0, 0, 1));
                br.rotate(new_angle, pos);
                br.render();

                auto [v1, v2] = br.getVerticesAsTuple();

                if (i <= rate) {
                    if (left_right == 0) {
                        recursive_branch_AD(v2, new_length, new_angle, k, true, rate, i + 1, angles_dist, gen);
                    } else {
                        recursive_branch_AD(v2, new_length, new_angle, k, false, rate, i + 1, angles_dist, gen);
                    }
                }
                left_right++;
            }
        }

        void recursive_branch_squared(Vertex pos_left, Vertex pos_right, int length, double angle, double k, bool isleft, int rate, int i) {

            int new_length = static_cast<int>(round(k * length));
            if (i != 0) {
                angle = angle + (isleft ? M_PI/4 : -M_PI/4);
            }

            std::vector branches = {
                    Rect(pos_left, Vertex(pos_left.x + new_length, pos_left.y + new_length)), 
                    Rect(pos_right, Vertex(pos_right.x - new_length, pos_right.y + new_length))
            };

            int left_right = 0;
            for (auto& br : branches) {
                double local_angle = (left_right == 0) ? angle + (M_PI/4) : angle - (M_PI/4);
                br.strokeColor(isleft ? Color(255, 255, 0, 1) : Color(255, 0, 0, 1));
                br.rotate(local_angle, (!left_right ? pos_left : pos_right));
                br.render();

                if (i <= rate) {
                    if (left_right == 0) {
                        recursive_branch_squared(br.getVertices()[3], br.getVertices()[2], new_length, angle, k, true, rate, i + 1);
                    } else {
                        recursive_branch_squared(br.getVertices()[2], br.getVertices()[3], new_length, angle, k, false, rate, i + 1);
                    }
                }
                left_right++;
            }
        }
        
    public:
        const Vertex& getPos() const {
            return position;
        }
        int getRate() const {
            return rate;
        }
        double getAngle() const {
            return angle;
        }

        void setPos(Vertex position) {
            this->position = position;
        }
        void setRate(int rate) {
            this->rate = rate;
        }
        void setAngle(double angle) {
            this->angle = angle;
        }
    
        Fractal(Vertex position = {0, 0}, int rate = 1, double angle = 0) : position{position}, rate{rate}, angle{angle} {};
        

        void draw_pythagorean_tree(int length, bool squared = false, 
            std::optional<std::pair<double, double>> angles_range = std::nullopt, 
            std::optional<std::mt19937> gen = std::nullopt) {

            double k = 1/sqrt(2);

            std::unique_ptr<Primitive> trunk = std::make_unique<Line>(this->position, Vertex(this->position.x, this->position.y + length));
            trunk->rotate(this->angle, Vertex(this->position.x, this->position.y + length));
            if (squared) {
                trunk = std::make_unique<Rect>(Vertex(this->position.x - length/2, this->position.y), length, length);
                trunk->rotate(this->angle, Vertex(this->position.x, this->position.y + length));
            }
            trunk->render();
            double start_y = this->position.y + length;

            if (squared) {
                recursive_branch_squared(trunk->getVertices()[3], trunk->getVertices()[2], length, this->angle, k, true, rate, 0);
            } else {
                std::optional<std::uniform_real_distribution<>> angles_dist = std::nullopt;
                if (angles_range.has_value()) {
                    angles_dist.emplace(angles_range.value().first, angles_range.value().second);
                }
                recursive_branch_AD(Vertex(this->position.x, start_y), length, this->angle, k, true, rate, 0, angles_dist, gen);
            }
        }

        std::string create_Lsystem(std::string axiom, const std::map<std::string, std::string>& rules) {
            std::string current = axiom;
            for (std::size_t i = 0; i < this->rate; i++) {
                std::string next_str;
                for (char& s : current) {
                    if (auto substr = rules.find(std::string(1, s)); substr != rules.end()) {
                        next_str += substr->second;
                    } else {
                        next_str += std::string(1, s);
                    }
                }
                current = next_str;
            }
            
            return current;
        }

        void draw_Lsystem_tree(std::string instructions, double step, double milliseconds) {
            std::stack<std::tuple<int, int, double, double>> pth_stack;
            double k1 = 0.1;
            double k2 = 1.0;
            auto [x, y] = this->position.as_tuple();
            double current_angle = 0;
            for (auto& s : instructions) {
                auto line = Line(Vertex(x, y), Vertex(x, y + step));
                auto leaf = Ellipsis(Vertex(x, y), 5, 10);
                switch (s) {
                    case 'F':
                        line.rotate(current_angle + this->angle, Vertex(x, y));
                        line.render();
                        std::tie(x, y) = line.getVertices()[1].as_tuple();
                        break;
                    case '+':
                        current_angle += M_PI/6;
                        break;
                    case '-':
                        current_angle += -M_PI/6;
                        break;
                    case '[':
                        pth_stack.push(std::make_tuple(static_cast<int>(x), static_cast<int>(y), current_angle, step));
                        step = step * 0.8 * (k1 + (milliseconds/480)*(k2 - k1));
                        break;
                    case ']':
                        std::tie(x, y, current_angle, step) = pth_stack.top();
                        leaf.strokeColor(Color(52, 201, 36, 1));
                        leaf.fillColor(Color(68, 148, 74, 1));
                        leaf.rotate(current_angle);
                        leaf.render();
                        pth_stack.pop();
                        break;
                    default:
                        throw std::runtime_error("Operation " + std::string(1, s) + " doesn't supported in Lsystem");
                }
            }
        }

};

#endif