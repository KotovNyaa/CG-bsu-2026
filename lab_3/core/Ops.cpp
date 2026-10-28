#include "Ops.h"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace Core {

Result Ops::step(int x1, int y1, int x2, int y2, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Line, Algo::Step};
    const int dx = x2 - x1;
    const int dy = y2 - y1;
    const int adx = std::abs(dx);
    const int ady = std::abs(dy);

    if (adx >= ady) {
        const int sx = dx >= 0 ? 1 : -1;
        const double k = dx == 0 ? 0.0 : static_cast<double>(dy) / dx;
        for (int i = 0; i <= adx; ++i) {
            const int cx = x1 + i * sx;
            const double cy = y1 + k * (cx - x1);
            const int ry = static_cast<int>(std::round(cy));
            res.pixels.push_back({cx, ry, 1.0});
            if (log) res.steps.push_back({i, cx, ry, cy - ry, "x++"});
        }
    } else {
        const int sy = dy >= 0 ? 1 : -1;
        const double inv = dy == 0 ? 0.0 : static_cast<double>(dx) / dy;
        for (int i = 0; i <= ady; ++i) {
            const int cy = y1 + i * sy;
            const double cx = x1 + inv * (cy - y1);
            const int rx = static_cast<int>(std::round(cx));
            res.pixels.push_back({rx, cy, 1.0});
            if (log) res.steps.push_back({i, rx, cy, cx - rx, "y++"});
        }
    }
    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

Result Ops::dda(int x1, int y1, int x2, int y2, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Line, Algo::Dda};
    const int dx = x2 - x1;
    const int dy = y2 - y1;
    const int len = std::max(std::abs(dx), std::abs(dy));

    if (len == 0) {
        res.pixels.push_back({x1, y1, 1.0});
        if (log) res.steps.push_back({0, x1, y1, 0.0, "dot"});
    } else {
        const double ix = static_cast<double>(dx) / len;
        const double iy = static_cast<double>(dy) / len;
        double cx = x1;
        double cy = y1;
        for (int i = 0; i <= len; ++i) {
            const int rx = static_cast<int>(std::round(cx));
            const int ry = static_cast<int>(std::round(cy));
            res.pixels.push_back({rx, ry, 1.0});
            if (log) res.steps.push_back({i, rx, ry, (cx - rx) + (cy - ry), "add"});
            cx += ix;
            cy += iy;
        }
    }
    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

Result Ops::bresenham(int x1, int y1, int x2, int y2, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Line, Algo::Bresenham};
    const int dx = std::abs(x2 - x1);
    const int dy = std::abs(y2 - y1);
    const int sx = x1 < x2 ? 1 : -1;
    const int sy = y1 < y2 ? 1 : -1;
    int cx = x1;
    int cy = y1;
    int step = 0;

    if (dx >= dy) {
        int e = 2 * dy - dx;
        for (int i = 0; i <= dx; ++i) {
            res.pixels.push_back({cx, cy, 1.0});
            if (log) res.steps.push_back({step++, cx, cy, static_cast<double>(e), e >= 0 ? "diag" : "straight"});
            if (e >= 0) {
                cy += sy;
                e += 2 * (dy - dx);
            } else {
                e += 2 * dy;
            }
            cx += sx;
        }
    } else {
        int e = 2 * dx - dy;
        for (int i = 0; i <= dy; ++i) {
            res.pixels.push_back({cx, cy, 1.0});
            if (log) res.steps.push_back({step++, cx, cy, static_cast<double>(e), e >= 0 ? "diag" : "straight"});
            if (e >= 0) {
                cx += sx;
                e += 2 * (dx - dy);
            } else {
                e += 2 * dx;
            }
            cy += sy;
        }
    }
    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

Result Ops::castle(int x1, int y1, int x2, int y2, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Line, Algo::Castle};
    const int dx = std::abs(x2 - x1);
    const int dy = std::abs(y2 - y1);
    const int sx = x1 < x2 ? 1 : -1;
    const int sy = y1 < y2 ? 1 : -1;

    if (dx == 0 && dy == 0) {
        res.pixels.push_back({x1, y1, 1.0});
        if (log) res.steps.push_back({0, x1, y1, 0.0, "dot"});
        return res;
    }

    const bool steep = dy > dx;
    const int a = steep ? dy : dx;
    const int b = steep ? dx : dy;
    std::string seq;

    if (b == 0) {
        seq.assign(a, 's');
    } else if (a == b) {
        seq.assign(a, 'd');
    } else {
        int x = a - b;
        int y = b;
        std::string m1 = "s";
        std::string m2 = "d";
        while (x != y) {
            if (x > y) {
                x -= y;
                m2 = m1 + m2;
            } else {
                y -= x;
                m1 = m2 + m1;
            }
        }
        const std::string unit = m2 + m1;
        for (int k = 0; k < x; ++k) seq += unit;
    }

    int cx = x1;
    int cy = y1;
    res.pixels.push_back({cx, cy, 1.0});
    if (log) res.steps.push_back({0, cx, cy, 0.0, "start"});

    for (size_t i = 0; i < seq.size(); ++i) {
        const char c = seq[i];
        if (!steep) {
            if (c == 's') cx += sx;
            else { cx += sx; cy += sy; }
        } else {
            if (c == 's') cy += sy;
            else { cx += sx; cy += sy; }
        }
        res.pixels.push_back({cx, cy, 1.0});
        if (log) res.steps.push_back({static_cast<int>(i + 1), cx, cy, 0.0, std::string(1, c)});
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

Result Ops::wu(int x1, int y1, int x2, int y2, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Line, Algo::Wu};
    const bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);
    int ax1 = x1, ay1 = y1, ax2 = x2, ay2 = y2;
    if (steep) {
        std::swap(ax1, ay1);
        std::swap(ax2, ay2);
    }
    const bool rev = ax1 > ax2;
    if (rev) {
        std::swap(ax1, ax2);
        std::swap(ay1, ay2);
    }

    const double dx = ax2 - ax1;
    const double dy = ay2 - ay1;
    const double grad = dx == 0.0 ? 1.0 : dy / dx;
    double y = ay1;
    int step = 0;

    for (int x = ax1; x <= ax2; ++x) {
        const int py = static_cast<int>(std::floor(y));
        const double f = y - py;
        const double a1 = 1.0 - f;
        const double a2 = f;

        if (steep) {
            res.pixels.push_back({py, x, a1});
            res.pixels.push_back({py + 1, x, a2});
            if (log) {
                res.steps.push_back({step++, py, x, a1, "wu-main"});
                res.steps.push_back({step++, py + 1, x, a2, "wu-sub"});
            }
        } else {
            res.pixels.push_back({x, py, a1});
            res.pixels.push_back({x, py + 1, a2});
            if (log) {
                res.steps.push_back({step++, x, py, a1, "wu-main"});
                res.steps.push_back({step++, x, py + 1, a2, "wu-sub"});
            }
        }
        y += grad;
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

Result Ops::circle(int xc, int yc, int r, bool log) {
    const auto t0 = std::chrono::high_resolution_clock::now();
    Result res{Prim::Circle, Algo::Circle};
    if (r <= 0) {
        res.pixels.push_back({xc, yc, 1.0});
        if (log) res.steps.push_back({0, xc, yc, 0.0, "dot"});
        return res;
    }

    int x = 0;
    int y = r;
    int e = 3 - 2 * r;
    int step = 0;

    auto put8 = [&](int cx, int cy, int eVal, const std::string &act) {
        const Pixel pts[8] = {
            {xc + cx, yc + cy, 1.0}, {xc + cx, yc - cy, 1.0},
            {xc - cx, yc + cy, 1.0}, {xc - cx, yc - cy, 1.0},
            {xc + cy, yc + cx, 1.0}, {xc + cy, yc - cx, 1.0},
            {xc - cy, yc + cx, 1.0}, {xc - cy, yc - cx, 1.0}
        };
        for (const auto &p : pts) res.pixels.push_back(p);
        if (log) res.steps.push_back({step++, xc + cx, yc + cy, static_cast<double>(eVal), act});
    };

    while (x <= y) {
        put8(x, y, e, e >= 0 ? "diag" : "straight");
        if (e >= 0) {
            e += 4 * (x - y) + 10;
            --y;
        } else {
            e += 4 * x + 6;
        }
        ++x;
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    res.time = std::chrono::duration<double, std::micro>(t1 - t0).count();
    return res;
}

}