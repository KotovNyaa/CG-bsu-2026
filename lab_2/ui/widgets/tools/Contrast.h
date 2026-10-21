#pragma once

#include <QWidget>

class Combo;
class Slider;
class QPushButton;

class Contrast : public QWidget {
  Q_OBJECT

public:
  explicit Contrast(QWidget *parent = nullptr);
  void setRange(int minVal, int maxVal);
  void resetState();

signals:
  void modeChanged(int mode);
  void rangeChanged(int minVal, int maxVal);
  void autoRequested();

private:
  Combo *m_combo{nullptr};
  Slider *m_minSlider{nullptr};
  Slider *m_maxSlider{nullptr};
  QPushButton *m_autoBtn{nullptr};
};