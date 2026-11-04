#include "Io.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace Core {

namespace {

std::string stripComments(const std::string &line) {
  std::string out;
  out.reserve(line.size());
  bool inStar = false;

  for (size_t i = 0; i < line.size(); ++i) {
    const char c = line[i];
    if (c == '*') {
      inStar = !inStar;
      continue;
    }
    if (inStar)
      continue;
    if (c == '#')
      break;
    if (c == '/' && i + 1 < line.size() && line[i + 1] == '/')
      break;
    out.push_back(c == ',' ? ' ' : c);
  }
  return out;
}

std::vector<double> parseNums(const std::string &line) {
  const std::string clean = stripComments(line);
  std::istringstream ss(clean);
  std::vector<double> nums;
  double v = 0.0;
  while (ss >> v) {
    nums.push_back(v);
  }
  return nums;
}

std::string fmtNum(double v) {
  if (std::abs(v) < 1e-9)
    v = 0.0;
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << v;
  std::string s = ss.str();
  if (s.find('.') != std::string::npos) {
    while (!s.empty() && s.back() == '0')
      s.pop_back();
    if (!s.empty() && s.back() == '.')
      s.push_back('0');
  }
  return s;
}

void pushUniquePt(std::vector<Point> &poly, const Point &p) {
  if (poly.empty() ||
      std::hypot(poly.back().x - p.x, poly.back().y - p.y) > 1e-7) {
    poly.push_back(p);
  }
}

} // namespace

std::vector<Segment> Io::parseSegments(const std::string &text) {
  std::istringstream ss(text);
  std::string line;
  std::vector<Segment> segs;

  while (std::getline(ss, line)) {
    const auto nums = parseNums(line);
    if (nums.size() >= 4) {
      segs.push_back({{nums[0], nums[1]}, {nums[2], nums[3]}});
    }
  }
  return segs;
}

std::vector<Point> Io::parsePolygon(const std::string &text) {
  std::istringstream ss(text);
  std::string line;
  std::vector<Point> poly;

  while (std::getline(ss, line)) {
    const auto nums = parseNums(line);
    if (nums.size() == 2 || nums.size() == 3) {
      pushUniquePt(poly, {nums[0], nums[1]});
    } else if (nums.size() >= 4) {
      pushUniquePt(poly, {nums[0], nums[1]});
      pushUniquePt(poly, {nums[2], nums[3]});
    }
  }
  if (poly.size() > 1 && std::hypot(poly.front().x - poly.back().x,
                                    poly.front().y - poly.back().y) < 1e-7) {
    poly.pop_back();
  }
  return poly;
}

std::string Io::formatSegments(const std::vector<Segment> &segs) {
  std::ostringstream ss;
  for (size_t i = 0; i < segs.size(); ++i) {
    const auto &s = segs[i];
    ss << fmtNum(s.p1.x) << "  " << fmtNum(s.p1.y) << "  " << fmtNum(s.p2.x)
       << "  " << fmtNum(s.p2.y);
    if (i + 1 < segs.size())
      ss << "\n";
  }
  return ss.str();
}

std::string Io::formatPolygon(const std::vector<Point> &poly) {
  std::ostringstream ss;
  for (size_t i = 0; i < poly.size(); ++i) {
    ss << fmtNum(poly[i].x) << "  " << fmtNum(poly[i].y);
    if (i + 1 < poly.size())
      ss << "\n";
  }
  return ss.str();
}

bool Io::loadFile(const std::string &path, FileData &out, std::string &err) {
  std::ifstream in(path);
  if (!in.is_open()) {
    err = "Failed to open file";
    return false;
  }

  std::vector<std::vector<double>> rows;
  std::string line;
  while (std::getline(in, line)) {
    auto nums = parseNums(line);
    if (!nums.empty()) {
      rows.push_back(std::move(nums));
    }
  }

  if (rows.size() < 2) {
    err = "Invalid file format (not enough rows)";
    return false;
  }

  size_t startIdx = 0;
  if (rows[0].size() == 1) {
    startIdx = 1;
  }

  if (rows.back().size() < 4 || startIdx >= rows.size() - 1) {
    err = "Missing clipping window [Xmin Ymin Xmax Ymax] at end of file";
    return false;
  }

  const auto &wRow = rows.back();
  out.win = Rect{wRow[0], wRow[1], wRow[2], wRow[3]}.normalized();
  out.segs.clear();
  out.poly.clear();
  out.hasPoly = false;

  for (size_t i = startIdx; i + 1 < rows.size(); ++i) {
    const auto &r = rows[i];
    if (r.size() >= 4) {
      out.segs.push_back({{r[0], r[1]}, {r[2], r[3]}});
      pushUniquePt(out.poly, {r[0], r[1]});
      pushUniquePt(out.poly, {r[2], r[3]});
    } else if (r.size() >= 2) {
      out.hasPoly = true;
      pushUniquePt(out.poly, {r[0], r[1]});
    }
  }

  if (out.poly.size() > 1 &&
      std::hypot(out.poly.front().x - out.poly.back().x,
                 out.poly.front().y - out.poly.back().y) < 1e-7) {
    out.poly.pop_back();
  }

  if (out.segs.empty() && out.poly.size() >= 2) {
    for (size_t i = 0; i < out.poly.size(); ++i) {
      out.segs.push_back({out.poly[i], out.poly[(i + 1) % out.poly.size()]});
    }
  }

  if (out.segs.empty() && out.poly.empty()) {
    err = "No geometry primitives found in file";
    return false;
  }

  return true;
}

bool Io::saveFile(const std::string &path, Mode mode, const Rect &win,
                  const std::vector<Segment> &segs,
                  const std::vector<Point> &poly, std::string &err) {
  std::ofstream out(path);
  if (!out.is_open()) {
    err = "Failed to open file for writing";
    return false;
  }

  const Rect w = win.normalized();

  if (mode == Mode::LiangBarsky) {
    out << segs.size() << "\n";
    for (const auto &s : segs) {
      out << fmtNum(s.p1.x) << " " << fmtNum(s.p1.y) << " " << fmtNum(s.p2.x)
          << " " << fmtNum(s.p2.y) << "\n";
    }
  } else {
    out << poly.size() << "\n";
    for (size_t i = 0; i < poly.size(); ++i) {
      const Point &p1 = poly[i];
      const Point &p2 = poly[(i + 1) % poly.size()];
      out << fmtNum(p1.x) << " " << fmtNum(p1.y) << " " << fmtNum(p2.x) << " "
          << fmtNum(p2.y) << "\n";
    }
  }

  out << fmtNum(w.xmin) << " " << fmtNum(w.ymin) << " " << fmtNum(w.xmax) << " "
      << fmtNum(w.ymax) << "\n";

  return true;
}

} // namespace Core
