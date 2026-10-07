#pragma once

#include <QWidget>

class QLabel;
class QSlider;
class QDoubleSpinBox;

class SliderRow : public QWidget {
  Q_OBJECT

public:
  explicit SliderRow(const QString &label, double min, double max, double step, int decimals, QWidget *parent = nullptr);

  void setValue(double val);
  double value() const;

signals:
  void valueChanged(double val);

private:
  QSlider *m_slider{nullptr};
  QDoubleSpinBox *m_spin{nullptr};
  double m_min{0.0};
  double m_max{100.0};
};