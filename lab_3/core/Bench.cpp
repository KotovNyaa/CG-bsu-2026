#include "Bench.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>

namespace Core {

Report Bench::run(Prim prim, int runs) {
    Report rep{prim, runs};

    if (prim == Prim::Line) {
        const int coords[8][4] = {
            {0, 0, 100, 30},
            {0, 0, 30, 100},
            {0, 0, -100, 30},
            {0, 0, 30, -100},
            {0, 0, -100, -30},
            {0, 0, -30, -100},
            {0, 0, 100, -30},
            {0, 0, -30, 100}
        };

        auto benchStep = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int r = 0; r < runs; ++r) {
                const auto &c = coords[r & 7];
                const int x1 = c[0], y1 = c[1], x2 = c[2], y2 = c[3];
                const int dx = x2 - x1, dy = y2 - y1;
                const int adx = std::abs(dx), ady = std::abs(dy);
                if (adx >= ady) {
                    const int sx = dx >= 0 ? 1 : -1;
                    const double k = dx == 0 ? 0.0 : static_cast<double>(dy) / dx;
                    for (int i = 0; i <= adx; ++i) {
                        const int cx = x1 + i * sx;
                        const int ry = static_cast<int>(std::round(y1 + k * (cx - x1)));
                        dummy += cx + ry;
                    }
                } else {
                    const int sy = dy >= 0 ? 1 : -1;
                    const double inv = dy == 0 ? 0.0 : static_cast<double>(dx) / dy;
                    for (int i = 0; i <= ady; ++i) {
                        const int cy = y1 + i * sy;
                        const int rx = static_cast<int>(std::round(x1 + inv * (cy - y1)));
                        dummy += rx + cy;
                    }
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Step, "Step-by-Step", ms, (ms * 1000.0) / runs, 1.0};
        };

        auto benchDda = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int r = 0; r < runs; ++r) {
                const auto &c = coords[r & 7];
                const int dx = c[2] - c[0], dy = c[3] - c[1];
                const int len = std::max(std::abs(dx), std::abs(dy));
                if (len > 0) {
                    const double ix = static_cast<double>(dx) / len;
                    const double iy = static_cast<double>(dy) / len;
                    double cx = c[0], cy = c[1];
                    for (int i = 0; i <= len; ++i) {
                        dummy += static_cast<int>(std::round(cx)) + static_cast<int>(std::round(cy));
                        cx += ix;
                        cy += iy;
                    }
                } else {
                    dummy += c[0] + c[1];
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Dda, "DDA", ms, (ms * 1000.0) / runs, 1.0};
        };

        auto benchBresenham = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int r = 0; r < runs; ++r) {
                const auto &c = coords[r & 7];
                const int x1 = c[0], y1 = c[1], x2 = c[2], y2 = c[3];
                const int dx = std::abs(x2 - x1), dy = std::abs(y2 - y1);
                const int sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1;
                int cx = x1, cy = y1;
                if (dx >= dy) {
                    int e = 2 * dy - dx;
                    for (int i = 0; i <= dx; ++i) {
                        dummy += cx + cy;
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
                        dummy += cx + cy;
                        if (e >= 0) {
                            cx += sx;
                            e += 2 * (dx - dy);
                        } else {
                            e += 2 * dx;
                        }
                        cy += sy;
                    }
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Bresenham, "Bresenham", ms, (ms * 1000.0) / runs, 1.0};
        };

        auto benchCastle = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int r = 0; r < runs; ++r) {
                const auto &c = coords[r & 7];
                const int dx = std::abs(c[2] - c[0]), dy = std::abs(c[3] - c[1]);
                const int sx = c[0] < c[2] ? 1 : -1, sy = c[1] < c[3] ? 1 : -1;
                const bool steep = dy > dx;
                const int a = steep ? dy : dx;
                const int b = steep ? dx : dy;
                std::string seq;
                if (b == 0) {
                    seq.assign(a, 's');
                } else if (a == b) {
                    seq.assign(a, 'd');
                } else {
                    int x = a - b, y = b;
                    std::string m1 = "s", m2 = "d";
                    while (x != y) {
                        if (x > y) { x -= y; m2 = m1 + m2; }
                        else { y -= x; m1 = m2 + m1; }
                    }
                    const std::string unit = m2 + m1;
                    for (int k = 0; k < x; ++k) seq += unit;
                }
                int cx = c[0], cy = c[1];
                dummy += cx + cy;
                for (char ch : seq) {
                    if (!steep) {
                        if (ch == 's') cx += sx;
                        else { cx += sx; cy += sy; }
                    } else {
                        if (ch == 's') cy += sy;
                        else { cx += sx; cy += sy; }
                    }
                    dummy += cx + cy;
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Castle, "Castle-Pitteway", ms, (ms * 1000.0) / runs, 1.0};
        };

        auto benchWu = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int r = 0; r < runs; ++r) {
                const auto &c = coords[r & 7];
                int ax1 = c[0], ay1 = c[1], ax2 = c[2], ay2 = c[3];
                const bool steep = std::abs(ay2 - ay1) > std::abs(ax2 - ax1);
                if (steep) { std::swap(ax1, ay1); std::swap(ax2, ay2); }
                if (ax1 > ax2) { std::swap(ax1, ax2); std::swap(ay1, ay2); }
                const double dx = ax2 - ax1, dy = ay2 - ay1;
                const double grad = dx == 0.0 ? 1.0 : dy / dx;
                double y = ay1;
                for (int x = ax1; x <= ax2; ++x) {
                    const int py = static_cast<int>(std::floor(y));
                    const double f = y - py;
                    dummy += x + py + static_cast<int>((1.0 - f) * 100) + static_cast<int>(f * 100);
                    y += grad;
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Wu, "Wu (Antialiased)", ms, (ms * 1000.0) / runs, 1.0};
        };

        rep.items.push_back(benchStep());
        rep.items.push_back(benchDda());
        rep.items.push_back(benchBresenham());
        rep.items.push_back(benchCastle());
        rep.items.push_back(benchWu());

        const double base = rep.items.front().total > 0.0 ? rep.items.front().total : 1.0;
        for (auto &it : rep.items) {
            it.boost = it.total > 0.0 ? base / it.total : 1.0;
        }
    } else {
        const int radii[4] = {15, 35, 75, 120};

        auto benchNaiveCircle = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < runs; ++i) {
                const int r = radii[i & 3];
                for (int x = 0; x <= r; ++x) {
                    const int y = static_cast<int>(std::round(std::sqrt(r * r - x * x)));
                    dummy += (x + y) + (x - y) + (-x + y) + (-x - y)
                           + (y + x) + (y - x) + (-y + x) + (-y - x);
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Circle, "Naive (sqrt)", ms, (ms * 1000.0) / runs, 1.0};
        };

        auto benchBresenhamCircle = [&]() -> BenchItem {
            volatile int dummy = 0;
            const auto t0 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < runs; ++i) {
                const int r = radii[i & 3];
                int x = 0, y = r;
                int e = 3 - 2 * r;
                while (x <= y) {
                    dummy += (x + y) + (x - y) + (-x + y) + (-x - y)
                           + (y + x) + (y - x) + (-y + x) + (-y - x);
                    if (e >= 0) {
                        e += 4 * (x - y) + 10;
                        --y;
                    } else {
                        e += 4 * x + 6;
                    }
                    ++x;
                }
            }
            const auto t1 = std::chrono::high_resolution_clock::now();
            const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return {Algo::Circle, "Bresenham Circle", ms, (ms * 1000.0) / runs, 1.0};
        };

        rep.items.push_back(benchNaiveCircle());
        rep.items.push_back(benchBresenhamCircle());

        const double base = rep.items.front().total > 0.0 ? rep.items.front().total : 1.0;
        for (auto &it : rep.items) {
            it.boost = it.total > 0.0 ? base / it.total : 1.0;
        }
    }

    return rep;
}

}