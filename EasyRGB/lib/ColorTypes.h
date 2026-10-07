#pragma once

namespace ColorLib {

struct Rgb {
  double r{0.0};
  double g{0.0};
  double b{0.0};
};

struct Cmyk {
  double c{0.0};
  double m{0.0};
  double y{0.0};
  double k{0.0};
};

struct Hls {
  double h{0.0};
  double l{0.0};
  double s{0.0};
};

}