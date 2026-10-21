#include "ColorMath.h"
#include <algorithm>
#include <cmath>

namespace ColorLib {

Hls ColorMath::rgbToHls(const Rgb &rgb) {
  const double max = std::max({rgb.r, rgb.g, rgb.b});
  const double min = std::min({rgb.r, rgb.g, rgb.b});
  const double delta = max - min;
  const double l = (max + min) * 0.5;

  if (delta <= 1e-9)
    return {0.0, l, 0.0};

  const double s =
      (l <= 0.5) ? (delta / (max + min)) : (delta / (2.0 - max - min));
  double h = 0.0;

  if (rgb.r >= max) {
    h = (rgb.g - rgb.b) / delta + (rgb.g < rgb.b ? 6.0 : 0.0);
  } else if (rgb.g >= max) {
    h = (rgb.b - rgb.r) / delta + 2.0;
  } else {
    h = (rgb.r - rgb.g) / delta + 4.0;
  }

  return {h * 60.0, std::clamp(l, 0.0, 1.0), std::clamp(s, 0.0, 1.0)};
}

double ColorMath::hueToRgb(double p, double q, double t) {
  if (t < 0.0) t += 1.0;
  if (t > 1.0) t -= 1.0;
  if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
  if (t < 0.5) return q;
  if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
  return p;
}

Rgb ColorMath::hlsToRgb(const Hls &hls) {
  if (hls.s <= 1e-9) return {hls.l, hls.l, hls.l};

  const double q =
      (hls.l < 0.5) ? (hls.l * (1.0 + hls.s)) : (hls.l + hls.s - hls.l * hls.s);
  const double p = 2.0 * hls.l - q;
  const double hk = hls.h / 360.0;

  return {std::clamp(hueToRgb(p, q, hk + 1.0 / 3.0), 0.0, 1.0),
          std::clamp(hueToRgb(p, q, hk), 0.0, 1.0),
          std::clamp(hueToRgb(p, q, hk - 1.0 / 3.0), 0.0, 1.0)};
}

}