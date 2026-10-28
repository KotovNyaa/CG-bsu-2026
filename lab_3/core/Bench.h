#pragma once

#include "Types.h"

namespace Core {

class Bench {
public:
    static Report run(Prim prim, int runs);
};

}