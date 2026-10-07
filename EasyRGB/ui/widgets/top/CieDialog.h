#pragma once

#include "../../../lib/ColorTypes.h"
#include <QDialog>
#include <QPixmap>

class QLabel;

class CieCanvas : public QWidget {
  Q_OBJECT

public:
  explicit CieCanvas(QWidget *parent = nullptr);
  void setColor(const ColorLib::Rgb &rgb, double x, double y);

protected:
  void paintEvent(QPaintEvent *) override;

private:
  void renderBackground();
  QPointF toCanvas(double x, double y) const;

  QPixmap m_bgCache;
  ColorLib::Rgb m_currentRgb{123.0 / 255.0, 123.0 / 255.0, 4.0 / 255.0};
  double m_curX{0.3127};
  double m_curY{0.3290};

  const double m_minX{0.0};
  const double m_maxX{0.8};
  const double m_minY{0.0};
  const double m_maxY{0.9};
  const int m_padding{34};
};

class CieDialog : public QDialog {
  Q_OBJECT

public:
  explicit CieDialog(QWidget *parent = nullptr);
  void setRgb(const ColorLib::Rgb &rgb);

private:
  CieCanvas *m_canvas{nullptr};
  QLabel *m_coordsLabel{nullptr};
  QWidget *m_swatch{nullptr};
};
