#pragma once

#include "ColorTypes.h"

namespace ColorLib {

class ColorMath {
public:
  static Hls rgbToHls(const Rgb &rgb);
  static Rgb hlsToRgb(const Hls &hls);

private:
  static double hueToRgb(double p, double q, double t);
};

}