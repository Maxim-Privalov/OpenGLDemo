#include <optional>
#include <vector>
#include <unordered_map>
#include "vlines.hpp"


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