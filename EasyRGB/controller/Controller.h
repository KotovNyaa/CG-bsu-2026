#pragma once

#include "../lib/ColorMath.h"
#include <QObject>

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(QObject *parent = nullptr);

  void setRgb(const ColorLib::Rgb &rgb, QObject *origin = nullptr);
  void setRgbChannel(int ch, double val, QObject *origin = nullptr);

  void setCmyk(const ColorLib::Cmyk &cmyk, QObject *origin = nullptr);
  void setCmykChannel(int ch, double val, QObject *origin = nullptr);

  void setHls(const ColorLib::Hls &hls, QObject *origin = nullptr);
  void setHlsChannel(int ch, double val, QObject *origin = nullptr);

  void setHsv(double h, double s, double v, QObject *origin = nullptr);
  void setHex(const QString &hex, QObject *origin = nullptr);

  ColorLib::Rgb rgb() const { return m_rgb; }

signals:
  void rgbUpdated(const ColorLib::Rgb &rgb, QObject *origin);
  void cmykUpdated(const ColorLib::Cmyk &cmyk, QObject *origin);
  void hlsUpdated(const ColorLib::Hls &hls, QObject *origin);
  void hexUpdated(const QString &hex, QObject *origin);
  void hsvUpdated(double h, double s, double v, QObject *origin);
  void reportUpdated(const QString &report);

private:
  void sync(QObject *origin);

  ColorLib::Rgb m_rgb{123.0 / 255.0, 123.0 / 255.0, 4.0 / 255.0};
  ColorLib::Cmyk m_cmyk{};
  ColorLib::Hls m_hls{};
  QString m_hex{};
};
