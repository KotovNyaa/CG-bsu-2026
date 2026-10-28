#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Core {

enum class Prim {
    Line,
    Circle
};

enum class Algo {
    Step,
    Dda,
    Bresenham,
    Castle,
    Wu,
    Circle
};

struct Pixel {
    int x{0};
    int y{0};
    double a{1.0};
};

struct Step {
    int i{0};
    int x{0};
    int y{0};
    double err{0.0};
    std::string act;
};

struct Result {
    Prim prim{Prim::Line};
    Algo algo{Algo::Bresenham};
    std::vector<Pixel> pixels;
    std::vector<Step> steps;
    double time{0.0};
};

struct BenchItem {
    Algo algo{Algo::Bresenham};
    std::string name;
    double total{0.0};
    double avg{0.0};
    double boost{1.0};
};

struct Report {
    Prim prim{Prim::Line};
    int runs{0};
    std::vector<BenchItem> items;
};

}