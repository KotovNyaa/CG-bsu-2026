#pragma once

#include "Types.h"
#include <string>
#include <vector>

namespace Core {

struct FileData {
  Rect win{3.0, 2.0, 7.0, 5.0};
  std::vector<Segment> segs;
  std::vector<Point> poly;
  bool hasPoly{false};
};

class Io {
public:
  static bool loadFile(const std::string &path, FileData &out,
                       std::string &err);
  static bool saveFile(const std::string &path, Mode mode, const Rect &win,
                       const std::vector<Segment> &segs,
                       const std::vector<Point> &poly, std::string &err);

  static std::vector<Segment> parseSegments(const std::string &text);
  static std::vector<Point> parsePolygon(const std::string &text);
  static std::string formatSegments(const std::vector<Segment> &segs);
  static std::string formatPolygon(const std::vector<Point> &poly);
};

} // namespace Core
