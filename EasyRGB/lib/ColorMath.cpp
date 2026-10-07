#include "ColorMath.h"
#include <algorithm>
#include <cmath>

namespace ColorLib {

Cmyk ColorMath::rgbToCmyk(const Rgb &rgb) {
  const double k = 1.0 - std::max({rgb.r, rgb.g, rgb.b});
  if (k >= 1.0)
    return {0.0, 0.0, 0.0, 1.0};
  const double d = 1.0 - k;
  return {(d - rgb.r) / d, (d - rgb.g) / d, (d - rgb.b) / d, k};
}

Rgb ColorMath::cmykToRgb(const Cmyk &c) {
  const double d = 1.0 - c.k;
  return {std::clamp((1.0 - c.c) * d, 0.0, 1.0),
          std::clamp((1.0 - c.m) * d, 0.0, 1.0),
          std::clamp((1.0 - c.y) * d, 0.0, 1.0)};
}

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
  if (t < 0.0)
    t += 1.0;
  if (t > 1.0)
    t -= 1.0;
  if (t < 1.0 / 6.0)
    return p + (q - p) * 6.0 * t;
  if (t < 0.5)
    return q;
  if (t < 2.0 / 3.0)
    return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
  return p;
}

Rgb ColorMath::hlsToRgb(const Hls &hls) {
  if (hls.s <= 1e-9)
    return {hls.l, hls.l, hls.l};

  const double q =
      (hls.l < 0.5) ? (hls.l * (1.0 + hls.s)) : (hls.l + hls.s - hls.l * hls.s);
  const double p = 2.0 * hls.l - q;
  const double hk = hls.h / 360.0;

  return {std::clamp(hueToRgb(p, q, hk + 1.0 / 3.0), 0.0, 1.0),
          std::clamp(hueToRgb(p, q, hk), 0.0, 1.0),
          std::clamp(hueToRgb(p, q, hk - 1.0 / 3.0), 0.0, 1.0)};
}

Rgb ColorMath::hsvToRgb(double h, double s, double v) {
  h = std::clamp(h, 0.0, 360.0);
  s = std::clamp(s, 0.0, 1.0);
  v = std::clamp(v, 0.0, 1.0);

  if (s <= 1e-9)
    return {v, v, v};

  const double hSector = (h >= 360.0 ? 0.0 : h) / 60.0;
  const int i = static_cast<int>(hSector);
  const double f = hSector - i;
  const double p = v * (1.0 - s);
  const double q = v * (1.0 - s * f);
  const double t = v * (1.0 - s * (1.0 - f));

  switch (i) {
  case 0:
    return {v, t, p};
  case 1:
    return {q, v, p};
  case 2:
    return {p, v, t};
  case 3:
    return {p, q, v};
  case 4:
    return {t, p, v};
  default:
    return {v, p, q};
  }
}

void ColorMath::rgbToHsv(const Rgb &rgb, double &h, double &s, double &v) {
  const double max = std::max({rgb.r, rgb.g, rgb.b});
  const double min = std::min({rgb.r, rgb.g, rgb.b});
  const double delta = max - min;
  v = max;

  if (max <= 1e-9 || delta <= 1e-9) {
    s = 0.0;
    h = 0.0;
    return;
  }

  s = delta / max;
  if (rgb.r >= max) {
    h = (rgb.g - rgb.b) / delta + (rgb.g < rgb.b ? 6.0 : 0.0);
  } else if (rgb.g >= max) {
    h = (rgb.b - rgb.r) / delta + 2.0;
  } else {
    h = (rgb.r - rgb.g) / delta + 4.0;
  }
  h *= 60.0;
}

QString ColorMath::rgbToHex(const Rgb &rgb) {
  return QString("#%1%2%3")
      .arg(static_cast<int>(std::round(rgb.r * 255.0)), 2, 16, QChar('0'))
      .arg(static_cast<int>(std::round(rgb.g * 255.0)), 2, 16, QChar('0'))
      .arg(static_cast<int>(std::round(rgb.b * 255.0)), 2, 16, QChar('0'))
      .toUpper();
}

Rgb ColorMath::hexToRgb(const QString &hex, bool &ok) {
  QString clean = hex.trimmed();
  if (clean.startsWith('#'))
    clean.remove(0, 1);
  if (clean.length() != 6) {
    ok = false;
    return {};
  }
  bool rOk, gOk, bOk;
  const int r = clean.mid(0, 2).toInt(&rOk, 16);
  const int g = clean.mid(2, 2).toInt(&gOk, 16);
  const int b = clean.mid(4, 2).toInt(&bOk, 16);
  ok = rOk && gOk && bOk;
  if (!ok)
    return {};
  return {r / 255.0, g / 255.0, b / 255.0};
}

void ColorMath::rgbToXy(const Rgb &rgb, double &x, double &y) {
  auto toLinear = [](double c) {
    return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
  };

  const double rLin = toLinear(rgb.r);
  const double gLin = toLinear(rgb.g);
  const double bLin = toLinear(rgb.b);

  const double X = rLin * 0.4124564 + gLin * 0.3575761 + bLin * 0.1804375;
  const double Y = rLin * 0.2126729 + gLin * 0.7151522 + bLin * 0.0721750;
  const double Z = rLin * 0.0193339 + gLin * 0.1191920 + bLin * 0.9503041;

  const double sum = X + Y + Z;
  if (sum <= 1e-9) {
    x = 0.3127;
    y = 0.3290;
  } else {
    x = X / sum;
    y = Y / sum;
  }
}

QString ColorMath::formatReport(const Rgb &rgb, const Cmyk &cmyk,
                                const Hls &hls, const QString &hex) {
  const int r = static_cast<int>(std::round(rgb.r * 255.0));
  const int g = static_cast<int>(std::round(rgb.g * 255.0));
  const int b = static_cast<int>(std::round(rgb.b * 255.0));

  return QString("sRGB         =  %1      %2        %3\n"
                 "sRGB   0-1.0 =  %4  %5  %6\n"
                 "HLS    0-1.0 =  %7  %8  %9    %10°\n"
                 "CMYK   0-1.0 =  %11  %12  %13  %14\n"
                 "CMYK   0-100 =  %15   %16   %17   %18\n\n"
                 "Pre-formatted code:\n"
                 "CSS, HTML    = %19\n"
                 "CSS rgb()    = rgb(%1, %2, %3)\n"
                 "CSS hsl()    = hsl(%10, %20%, %21%)\n"
                 "openGL       = glColor3f( %4f, %5f, %6f );\n"
                 "Java         = new Color( %1, %2, %3 )\n"
                 ".NET         = Color.FromArgb( 255, %1, %2, %3 );")
      .arg(r, 3)
      .arg(g, 3)
      .arg(b, 3)
      .arg(rgb.r, 7, 'f', 5)
      .arg(rgb.g, 7, 'f', 5)
      .arg(rgb.b, 7, 'f', 5)
      .arg(hls.h / 360.0, 7, 'f', 5)
      .arg(hls.l, 7, 'f', 5)
      .arg(hls.s, 7, 'f', 5)
      .arg(hls.h, 5, 'f', 2)
      .arg(cmyk.c, 7, 'f', 5)
      .arg(cmyk.m, 7, 'f', 5)
      .arg(cmyk.y, 7, 'f', 5)
      .arg(cmyk.k, 7, 'f', 5)
      .arg(cmyk.c * 100.0, 7, 'f', 3)
      .arg(cmyk.m * 100.0, 7, 'f', 3)
      .arg(cmyk.y * 100.0, 7, 'f', 3)
      .arg(cmyk.k * 100.0, 7, 'f', 3)
      .arg(hex)
      .arg(hls.s * 100.0, 4, 'f', 1)
      .arg(hls.l * 100.0, 4, 'f', 1);
}

} // namespace ColorLib
