#pragma once

#include <QWidget>

class QSlider;
class QSpinBox;

class Slider : public QWidget {
  Q_OBJECT

public:
  explicit Slider(const QString &label, int min, int max, int val, QWidget *parent = nullptr);
  void setValue(int val);
  int value() const;

signals:
  void valueChanged(int val);

private:
  QSlider *m_slider{nullptr};
  QSpinBox *m_spin{nullptr};
};