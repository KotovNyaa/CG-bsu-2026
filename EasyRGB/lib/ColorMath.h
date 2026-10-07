#pragma once

#include "ColorTypes.h"
#include <QString>

namespace ColorLib {

class ColorMath {
public:
  static Cmyk rgbToCmyk(const Rgb &rgb);
  static Rgb cmykToRgb(const Cmyk &cmyk);

  static Hls rgbToHls(const Rgb &rgb);
  static Rgb hlsToRgb(const Hls &hls);

  static Rgb hsvToRgb(double h, double s, double v);
  static void rgbToHsv(const Rgb &rgb, double &h, double &s, double &v);

  static QString rgbToHex(const Rgb &rgb);
  static Rgb hexToRgb(const QString &hex, bool &ok);

  static void rgbToXy(const Rgb &rgb, double &x, double &y);

  static QString formatReport(const Rgb &rgb, const Cmyk &cmyk, const Hls &hls,
                              const QString &hex);

private:
  static double hueToRgb(double p, double q, double t);
};

} // namespace ColorLib
