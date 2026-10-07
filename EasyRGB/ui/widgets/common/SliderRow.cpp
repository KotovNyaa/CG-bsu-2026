#include "SliderRow.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

SliderRow::SliderRow(const QString &label, double min, double max, double step, int decimals, QWidget *parent)
    : QWidget(parent), m_min(min), m_max(max) {
  setFixedHeight(28);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  auto *lbl = new QLabel(label, this);
  lbl->setObjectName("rowLabel");
  lbl->setFixedWidth(14);

  m_slider = new QSlider(Qt::Horizontal, this);
  m_slider->setRange(0, 1000);

  m_spin = new QDoubleSpinBox(this);
  m_spin->setRange(min, max);
  m_spin->setSingleStep(step);
  m_spin->setDecimals(decimals);
  m_spin->setFixedWidth(64);

  layout->addWidget(lbl);
  layout->addWidget(m_slider, 1);
  layout->addWidget(m_spin);

  connect(m_slider, &QSlider::valueChanged, this, [this](int pos) {
    const double val = m_min + (pos / 1000.0) * (m_max - m_min);
    const QSignalBlocker b(m_spin);
    m_spin->setValue(val);
    emit valueChanged(val);
  });

  connect(m_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
    const QSignalBlocker b(m_slider);
    m_slider->setValue(static_cast<int>(((val - m_min) / (m_max - m_min)) * 1000.0));
    emit valueChanged(val);
  });
}

void SliderRow::setValue(double val) {
  const QSignalBlocker b1(m_slider);
  const QSignalBlocker b2(m_spin);
  m_spin->setValue(val);
  m_slider->setValue(static_cast<int>(((val - m_min) / (m_max - m_min)) * 1000.0));
}

double SliderRow::value() const { return m_spin->value(); }