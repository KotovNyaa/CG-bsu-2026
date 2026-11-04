#include "Ops.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace Core {

namespace {

constexpr double kEps = 1e-9;

std::string fmt(double v, int prec = 2) {
  if (std::abs(v) < 1e-9)
    v = 0.0;
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(prec) << v;
  return ss.str();
}

std::string fmtPt(const Point &p) {
  return "(" + fmt(p.x) + ", " + fmt(p.y) + ")";
}

bool samePt(const Point &a, const Point &b) {
  return std::hypot(a.x - b.x, a.y - b.y) < 1e-7;
}

enum class Bound { Left, Right, Bottom, Top };

bool isInside(const Point &p, Bound b, const Rect &w) {
  switch (b) {
  case Bound::Left:
    return p.x >= w.xmin - kEps;
  case Bound::Right:
    return p.x <= w.xmax + kEps;
  case Bound::Bottom:
    return p.y >= w.ymin - kEps;
  case Bound::Top:
    return p.y <= w.ymax + kEps;
  }
  return false;
}

Point intersect(const Point &s, const Point &p, Bound b, const Rect &w) {
  const double dx = p.x - s.x;
  const double dy = p.y - s.y;

  switch (b) {
  case Bound::Left: {
    const double y = std::abs(dx) < kEps ? s.y : s.y + dy * (w.xmin - s.x) / dx;
    return {w.xmin, y};
  }
  case Bound::Right: {
    const double y = std::abs(dx) < kEps ? s.y : s.y + dy * (w.xmax - s.x) / dx;
    return {w.xmax, y};
  }
  case Bound::Bottom: {
    const double x = std::abs(dy) < kEps ? s.x : s.x + dx * (w.ymin - s.y) / dy;
    return {x, w.ymin};
  }
  case Bound::Top: {
    const double x = std::abs(dy) < kEps ? s.x : s.x + dx * (w.ymax - s.y) / dy;
    return {x, w.ymax};
  }
  }
  return s;
}

void pushUnique(std::vector<Point> &pts, const Point &p) {
  if (pts.empty() || !samePt(pts.back(), p)) {
    pts.push_back(p);
  }
}

} // namespace

bool Ops::isConvex(const std::vector<Point> &poly) {
  const size_t n = poly.size();
  if (n < 3)
    return false;

  bool hasPos = false;
  bool hasNeg = false;

  for (size_t i = 0; i < n; ++i) {
    const Point &a = poly[i];
    const Point &b = poly[(i + 1) % n];
    const Point &c = poly[(i + 2) % n];

    const double cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
    if (cross > 1e-7)
      hasPos = true;
    if (cross < -1e-7)
      hasNeg = true;
    if (hasPos && hasNeg)
      return false;
  }
  return true;
}

Result Ops::liangBarsky(const Rect &win, const std::vector<Segment> &segs,
                        bool log) {
  const auto t0 = std::chrono::high_resolution_clock::now();

  const Rect w = win.normalized();
  Result res;
  res.mode = Mode::LiangBarsky;
  res.win = w;
  res.total = static_cast<int>(segs.size());

  std::ostringstream logSs;
  if (log) {
    logSs << "=== АЛГОРИТМ ЛИАНГА-БАРСКИ ===\n";
    logSs << "Окно: X[" << fmt(w.xmin) << " .. " << fmt(w.xmax) << "], Y["
          << fmt(w.ymin) << " .. " << fmt(w.ymax) << "]\n\n";
  }

  int stepIdx = 0;
  const char *bNames[4] = {"i=1 (Нижн.)", "i=2 (Лев.)", "i=3 (Верх.)",
                           "i=4 (Прав.)"};

  for (size_t idx = 0; idx < segs.size(); ++idx) {
    const Segment &seg = segs[idx];
    const double x1 = seg.p1.x;
    const double y1 = seg.p1.y;
    const double x2 = seg.p2.x;
    const double y2 = seg.p2.y;
    const double dx = x2 - x1;
    const double dy = y2 - y1;

    const double S[4] = {-dy, -dx, dy, dx};
    const double Q[4] = {y1 - w.ymin, x1 - w.xmin, w.ymax - y1, w.xmax - x1};

    double tIn = 0.0;
    double tOut = 1.0;
    bool rejected = false;

    if (log) {
      logSs << "Отрезок #" << (idx + 1) << ": P1" << fmtPt(seg.p1) << " -> P2"
            << fmtPt(seg.p2) << "  (dx=" << fmt(dx) << ", dy=" << fmt(dy)
            << ")\n";
    }

    for (int i = 0; i < 4; ++i) {
      const std::string tag = "#" + std::to_string(idx + 1) + " " + bNames[i];
      if (std::abs(S[i]) < kEps) {
        const bool out = Q[i] < -kEps;
        if (log) {
          const std::string calc = "S" + std::to_string(i + 1) + "=0, Q" +
                                   std::to_string(i + 1) + "=" + fmt(Q[i]);
          const std::string act =
              out ? "Паралл. (ВНЕ -> отброс)" : "Паралл. (ВНУТРИ)";
          res.steps.push_back({stepIdx++, tag, calc, act, x1, y1});
          logSs << "  " << bNames[i] << ": " << calc << " => " << act << "\n";
        }
        if (out) {
          rejected = true;
          break;
        }
      } else {
        const double ti = Q[i] / S[i];
        const bool entering = S[i] < 0.0;
        if (entering) {
          tIn = std::max(tIn, ti);
        } else {
          tOut = std::min(tOut, ti);
        }

        if (log) {
          const double tc = std::clamp(ti, 0.0, 1.0);
          const double px = x1 + tc * dx;
          const double py = y1 + tc * dy;
          const std::string calc = "S" + std::to_string(i + 1) + "=" +
                                   fmt(S[i]) + ", Q" + std::to_string(i + 1) +
                                   "=" + fmt(Q[i]) + " -> t" +
                                   std::to_string(i + 1) + "=" + fmt(ti);
          const std::string act = (entering ? "Вход: t_вх=" : "Выход: t_вых=") +
                                  fmt(entering ? tIn : tOut);
          res.steps.push_back({stepIdx++, tag, calc, act, px, py});
          logSs << "  " << bNames[i] << ": " << calc << " ("
                << (entering ? "S<0 вход" : "S>0 выход") << ") => "
                << "t_вх=" << fmt(tIn) << ", t_вых=" << fmt(tOut) << "\n";
        }
      }
    }

    ClipSeg cs;
    cs.id = static_cast<int>(idx + 1);
    cs.orig = seg;
    cs.tIn = tIn;
    cs.tOut = tOut;

    if (!rejected && tIn <= tOut + kEps) {
      if (tIn > tOut)
        tIn = tOut;
      cs.tIn = tIn;
      cs.tOut = tOut;
      cs.visible = true;
      cs.clip.p1 = {x1 + tIn * dx, y1 + tIn * dy};
      cs.clip.p2 = {x1 + tOut * dx, y1 + tOut * dy};
      ++res.visible;

      if (log) {
        const std::string tag = "#" + std::to_string(idx + 1) + " Итог";
        const std::string calc = "t_вх=" + fmt(tIn) + " <= t_вых=" + fmt(tOut);
        const std::string act =
            "Видим: Pвх" + fmtPt(cs.clip.p1) + " -> Pвых" + fmtPt(cs.clip.p2);
        res.steps.push_back(
            {stepIdx++, tag, calc, act, cs.clip.p1.x, cs.clip.p1.y});
        logSs << "  => ВИДИМ: Pвх" << fmtPt(cs.clip.p1) << " [t_вх=" << fmt(tIn)
              << "], Pвых" << fmtPt(cs.clip.p2) << " [t_вых=" << fmt(tOut)
              << "]\n\n";
      }
    } else {
      cs.visible = false;
      if (log) {
        const std::string tag = "#" + std::to_string(idx + 1) + " Итог";
        const std::string calc =
            rejected ? "Q_i < 0"
                     : ("t_вх=" + fmt(tIn) + " > t_вых=" + fmt(tOut));
        const std::string act = "Отрезок полностью невидим";
        res.steps.push_back({stepIdx++, tag, calc, act, x1, y1});
        logSs << "  => НЕВИДИМ (" << calc << ")\n\n";
      }
    }

    res.segs.push_back(cs);
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
  res.log = logSs.str();
  return res;
}

Result Ops::sutherlandHodgman(const Rect &win, const std::vector<Point> &poly,
                              bool log) {
  const auto t0 = std::chrono::high_resolution_clock::now();

  const Rect w = win.normalized();
  Result res;
  res.mode = Mode::SutherlandHodgman;
  res.win = w;
  res.origPoly = poly;
  res.total = static_cast<int>(poly.size());
  res.convex = isConvex(poly);

  std::ostringstream logSs;
  if (log) {
    logSs << "=== АЛГОРИТМ САЗЕРЛЕНДА-ХОДЖМАНА ===\n";
    logSs << "Окно: X[" << fmt(w.xmin) << " .. " << fmt(w.xmax) << "], Y["
          << fmt(w.ymin) << " .. " << fmt(w.ymax) << "]\n";
    logSs << "Исходных вершин: " << poly.size() << " ("
          << (res.convex ? "Выпуклый" : "Невыпуклый!") << ")\n\n";
  }

  if (poly.size() < 3) {
    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    res.log = logSs.str() + "Недостаточно вершин для многоугольника (< 3).\n";
    return res;
  }

  const Bound bounds[4] = {Bound::Left, Bound::Right, Bound::Bottom,
                           Bound::Top};
  const char *bTitles[4] = {"Этап 1: Левая граница (x >= Xmin)",
                            "Этап 2: Правая граница (x <= Xmax)",
                            "Этап 3: Нижняя граница (y >= Ymin)",
                            "Этап 4: Верхняя граница (y <= Ymax)"};
  const char *bShort[4] = {"Лев.", "Прав.", "Нижн.", "Верх."};

  std::vector<Point> cur = poly;
  int stepIdx = 0;

  for (int b = 0; b < 4; ++b) {
    if (log) {
      logSs << bTitles[b] << " (входных вершин: " << cur.size() << ")\n";
    }

    if (cur.empty()) {
      if (log)
        logSs << "  Список вершин пуст.\n\n";
      break;
    }

    std::vector<Point> next;
    const size_t n = cur.size();

    for (size_t i = 0; i < n; ++i) {
      const Point &s = cur[i];
      const Point &p = cur[(i + 1) % n];
      const bool sIn = isInside(s, bounds[b], w);
      const bool pIn = isInside(p, bounds[b], w);

      const std::string tag = std::string(bShort[b]) + " P" +
                              std::to_string(i + 1) + "->P" +
                              std::to_string((i + 1) % n + 1);
      const std::string edgeStr = fmtPt(s) + " -> " + fmtPt(p);

      if (sIn && pIn) {
        pushUnique(next, s);
        if (log) {
          const std::string act = "Случ. 1 (Вн->Вн): +S" + fmtPt(s);
          res.steps.push_back({stepIdx++, tag, edgeStr, act, s.x, s.y});
          logSs << "  Ребро " << edgeStr << " => " << act << "\n";
        }
      } else if (sIn && !pIn) {
        const Point hit = intersect(s, p, bounds[b], w);
        pushUnique(next, s);
        pushUnique(next, hit);
        if (log) {
          const std::string act =
              "Случ. 2 (Выход): +S" + fmtPt(s) + ", +I" + fmtPt(hit);
          res.steps.push_back({stepIdx++, tag, edgeStr, act, hit.x, hit.y});
          logSs << "  Ребро " << edgeStr << " => " << act << "\n";
        }
      } else if (!sIn && !pIn) {
        if (log) {
          const std::string act = "Случ. 3 (Вне->Вне): пропуск";
          res.steps.push_back({stepIdx++, tag, edgeStr, act, s.x, s.y});
          logSs << "  Ребро " << edgeStr << " => " << act << "\n";
        }
      } else {
        const Point hit = intersect(s, p, bounds[b], w);
        pushUnique(next, hit);
        if (log) {
          const std::string act = "Случ. 4 (Вход): +I" + fmtPt(hit);
          res.steps.push_back({stepIdx++, tag, edgeStr, act, hit.x, hit.y});
          logSs << "  Ребро " << edgeStr << " => " << act << "\n";
        }
      }
    }

    if (next.size() > 1 && samePt(next.front(), next.back())) {
      next.pop_back();
    }

    cur = std::move(next);
    if (log) {
      logSs << "  Промежуточный контур (" << cur.size() << " верш.): ";
      for (const auto &pt : cur)
        logSs << fmtPt(pt) << " ";
      logSs << "\n\n";
    }
  }

  if (cur.size() >= 3) {
    res.clipPoly = std::move(cur);
    res.visible = static_cast<int>(res.clipPoly.size());
  } else {
    res.visible = 0;
  }

  if (log) {
    logSs << "=== ИТОГ ОТСЕЧЕНИЯ: " << res.visible << " вершин ===\n";
    for (size_t i = 0; i < res.clipPoly.size(); ++i) {
      logSs << "  V" << (i + 1) << " = " << fmtPt(res.clipPoly[i]) << "\n";
    }
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
  res.log = logSs.str();
  return res;
}

} // namespace Core
