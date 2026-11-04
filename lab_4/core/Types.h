#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace Core {

enum class Mode { LiangBarsky, SutherlandHodgman };

struct Point {
  double x{0.0};
  double y{0.0};
};

struct Segment {
  Point p1;
  Point p2;
};

struct Rect {
  double xmin{3.0};
  double ymin{2.0};
  double xmax{7.0};
  double ymax{5.0};

  Rect normalized() const {
    return {std::min(xmin, xmax), std::min(ymin, ymax), std::max(xmin, xmax),
            std::max(ymin, ymax)};
  }
};

struct ClipSeg {
  int id{0};
  Segment orig;
  Segment clip;
  double tIn{0.0};
  double tOut{1.0};
  bool visible{false};
};

struct Step {
  int i{0};
  std::string stage;
  std::string calc;
  std::string act;
  double x{0.0};
  double y{0.0};
};

struct Result {
  Mode mode{Mode::LiangBarsky};
  Rect win;
  std::vector<ClipSeg> segs;
  std::vector<Point> origPoly;
  std::vector<Point> clipPoly;
  bool convex{true};
  std::vector<Step> steps;
  std::string log;
  int total{0};
  int visible{0};
  double time{0.0};
};

} // namespace Core
