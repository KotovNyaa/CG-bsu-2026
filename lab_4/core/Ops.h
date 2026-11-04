#pragma once

#include "Types.h"

namespace Core {

class Ops {
public:
  static Result liangBarsky(const Rect &win, const std::vector<Segment> &segs,
                            bool log = true);
  static Result sutherlandHodgman(const Rect &win,
                                  const std::vector<Point> &poly,
                                  bool log = true);
  static bool isConvex(const std::vector<Point> &poly);
};

} // namespace Core
