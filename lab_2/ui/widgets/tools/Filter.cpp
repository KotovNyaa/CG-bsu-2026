#include "Filter.h"
#include "../common/Combo.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

Filter::Filter(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(6);

  auto *title = new QLabel("ORDER FILTERS", this);
  title->setObjectName("sectionTitle");
  layout->addWidget(title);

  m_typeCombo = new Combo(this);
  m_typeCombo->addItem("None (Bypass)");
  m_typeCombo->addItem("Median Filter");
  m_typeCombo->addItem("Minimum Filter");
  m_typeCombo->addItem("Maximum Filter");

  m_sizeCombo = new Combo(this);
  m_sizeCombo->addItem("3 × 3 (Radius 1)");
  m_sizeCombo->addItem("5 × 5 (Radius 2)");
  m_sizeCombo->addItem("7 × 7 (Radius 3)");

  layout->addWidget(m_typeCombo);
  layout->addWidget(m_sizeCombo);

  connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this]() { emitChange(); });
  connect(m_sizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this]() { emitChange(); });
}

void Filter::emitChange() {
  emit filterChanged(m_typeCombo->currentIndex(), m_sizeCombo->currentIndex() + 1);
}

void Filter::resetState() {
  const QSignalBlocker b1(m_typeCombo);
  const QSignalBlocker b2(m_sizeCombo);
  m_typeCombo->setCurrentIndex(0);
  m_sizeCombo->setCurrentIndex(0);
}