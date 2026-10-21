#include "Contrast.h"
#include "../common/Combo.h"
#include "../common/Slider.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

Contrast::Contrast(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(6);

  auto *topRow = new QHBoxLayout();
  auto *title = new QLabel("CONTRAST & EQUALIZATION", this);
  title->setObjectName("sectionTitle");

  m_autoBtn = new QPushButton("Auto", this);
  m_autoBtn->setFocusPolicy(Qt::NoFocus);
  m_autoBtn->setFixedHeight(22);

  topRow->addWidget(title);
  topRow->addStretch(1);
  topRow->addWidget(m_autoBtn);
  layout->addLayout(topRow);

  m_combo = new Combo(this);
  m_combo->addItem("None (Bypass)");
  m_combo->addItem("Linear Stretch");
  m_combo->addItem("Equalize RGB");
  m_combo->addItem("Equalize HLS");
  layout->addWidget(m_combo);

  m_minSlider = new Slider("Min", 0, 255, 0, this);
  m_maxSlider = new Slider("Max", 0, 255, 255, this);
  m_minSlider->setEnabled(false);
  m_maxSlider->setEnabled(false);

  layout->addWidget(m_minSlider);
  layout->addWidget(m_maxSlider);

  auto onSlider = [this]() {
    if (m_combo->currentIndex() != 1) {
      const QSignalBlocker b(m_combo);
  m_combo->setCurrentIndex(1);
}    m_minSlider->setEnabled(true);
    m_maxSlider->setEnabled(true);
    emit rangeChanged(m_minSlider->value(), m_maxSlider->value());
  };

  connect(m_minSlider, &Slider::valueChanged, this, onSlider);
  connect(m_maxSlider, &Slider::valueChanged, this, onSlider);
  connect(m_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int mode) {
            m_minSlider->setEnabled(mode == 1);
            m_maxSlider->setEnabled(mode == 1);
            emit modeChanged(mode);
          });
  connect(m_autoBtn, &QPushButton::clicked, this, &Contrast::autoRequested);
}

void Contrast::setRange(int minVal, int maxVal) {
  const QSignalBlocker b1(m_minSlider);
  const QSignalBlocker b2(m_maxSlider);
  const QSignalBlocker b3(m_combo);
  m_minSlider->setEnabled(true);
  m_maxSlider->setEnabled(true);
  m_minSlider->setValue(minVal);
  m_maxSlider->setValue(maxVal);
  m_combo->setCurrentIndex(1);
}

void Contrast::resetState() {
  const QSignalBlocker b1(m_minSlider);
  const QSignalBlocker b2(m_maxSlider);
  const QSignalBlocker b3(m_combo);
  m_minSlider->setValue(0);
  m_maxSlider->setValue(255);
  m_minSlider->setEnabled(false);
  m_maxSlider->setEnabled(false);
  m_combo->setCurrentIndex(0);
}