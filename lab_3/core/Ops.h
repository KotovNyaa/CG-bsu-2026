#pragma once

#include "Types.h"

namespace Core {

class Ops {
public:
    static Result step(int x1, int y1, int x2, int y2, bool log = false);
    static Result dda(int x1, int y1, int x2, int y2, bool log = false);
    static Result bresenham(int x1, int y1, int x2, int y2, bool log = false);
    static Result castle(int x1, int y1, int x2, int y2, bool log = false);
    static Result wu(int x1, int y1, int x2, int y2, bool log = false);
    static Result circle(int xc, int yc, int r, bool log = false);
};

}