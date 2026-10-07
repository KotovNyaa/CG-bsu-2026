#pragma once

#include <QWidget>

class SvCanvas;
class HueBar;

class Picker2D : public QWidget {
  Q_OBJECT

public:
  explicit Picker2D(QWidget *parent = nullptr);
  void setHsv(double h, double s, double v);

signals:
  void hsvChanged(double h, double s, double v);

private:
  SvCanvas *m_svCanvas{nullptr};
  HueBar *m_hueBar{nullptr};
  double m_h{0.0};
  double m_s{1.0};
  double m_v{1.0};
};