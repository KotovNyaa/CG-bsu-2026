#include "Controller.h"

Controller::Controller(QObject *parent) : QObject(parent) {
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(nullptr);
}

void Controller::setRgb(const ColorLib::Rgb &rgb, QObject *origin) {
  m_rgb = {std::clamp(rgb.r, 0.0, 1.0), std::clamp(rgb.g, 0.0, 1.0),
           std::clamp(rgb.b, 0.0, 1.0)};
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(origin);
}

void Controller::setRgbChannel(int ch, double val, QObject *origin) {
  val = std::clamp(val, 0.0, 1.0);
  if (ch == 0)
    m_rgb.r = val;
  else if (ch == 1)
    m_rgb.g = val;
  else if (ch == 2)
    m_rgb.b = val;
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(origin);
}

void Controller::setCmyk(const ColorLib::Cmyk &cmyk, QObject *origin) {
  m_cmyk = {std::clamp(cmyk.c, 0.0, 1.0), std::clamp(cmyk.m, 0.0, 1.0),
            std::clamp(cmyk.y, 0.0, 1.0), std::clamp(cmyk.k, 0.0, 1.0)};
  m_rgb = ColorLib::ColorMath::cmykToRgb(m_cmyk);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(origin);
}

void Controller::setCmykChannel(int ch, double val, QObject *origin) {
  val = std::clamp(val, 0.0, 1.0);
  if (ch == 0)
    m_cmyk.c = val;
  else if (ch == 1)
    m_cmyk.m = val;
  else if (ch == 2)
    m_cmyk.y = val;
  else if (ch == 3)
    m_cmyk.k = val;
  m_rgb = ColorLib::ColorMath::cmykToRgb(m_cmyk);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(origin);
}

void Controller::setHls(const ColorLib::Hls &hls, QObject *origin) {
  m_hls = {std::clamp(hls.h, 0.0, 360.0), std::clamp(hls.l, 0.0, 1.0),
           std::clamp(hls.s, 0.0, 1.0)};
  m_rgb = ColorLib::ColorMath::hlsToRgb(m_hls);
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  sync(origin);
}

void Controller::setHlsChannel(int ch, double val, QObject *origin) {
  if (ch == 0)
    m_hls.h = std::clamp(val, 0.0, 360.0);
  else if (ch == 1)
    m_hls.l = std::clamp(val, 0.0, 1.0);
  else if (ch == 2)
    m_hls.s = std::clamp(val, 0.0, 1.0);
  m_rgb = ColorLib::ColorMath::hlsToRgb(m_hls);
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  sync(origin);
}

void Controller::setHsv(double h, double s, double v, QObject *origin) {
  m_rgb = ColorLib::ColorMath::hsvToRgb(h, s, v);
  m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
  m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
  sync(origin);
}

void Controller::setHex(const QString &hex, QObject *origin) {
  bool ok = false;
  const auto rgb = ColorLib::ColorMath::hexToRgb(hex, ok);
  if (ok) {
    m_rgb = rgb;
    m_cmyk = ColorLib::ColorMath::rgbToCmyk(m_rgb);
    m_hls = ColorLib::ColorMath::rgbToHls(m_rgb);
    sync(origin);
  }
}

void Controller::sync(QObject *origin) {
  m_hex = ColorLib::ColorMath::rgbToHex(m_rgb);

  double h, s, v;
  ColorLib::ColorMath::rgbToHsv(m_rgb, h, s, v);

  emit rgbUpdated(m_rgb, origin);
  emit cmykUpdated(m_cmyk, origin);
  emit hlsUpdated(m_hls, origin);
  emit hexUpdated(m_hex, origin);
  emit hsvUpdated(h, s, v, origin);
  emit reportUpdated(
      ColorLib::ColorMath::formatReport(m_rgb, m_cmyk, m_hls, m_hex));
}
