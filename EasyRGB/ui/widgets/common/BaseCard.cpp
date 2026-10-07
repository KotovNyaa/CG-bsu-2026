#include "BaseCard.h"
#include <QLabel>

BaseCard::BaseCard(const QString &title, const QString &inputLabel,
                   QWidget *parent)
    : QFrame(parent) {
  setObjectName("modelCard");
  setFixedSize(332, 242);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(14, 12, 14, 12);
  layout->setSpacing(8);

  auto *titleLabel = new QLabel(title, this);
  titleLabel->setObjectName("cardTitle");
  layout->addWidget(titleLabel);

  m_rowsLayout = new QVBoxLayout();
  m_rowsLayout->setSpacing(6);
  layout->addLayout(m_rowsLayout);

  layout->addStretch(1);

  m_inputRow = new InputRow(inputLabel, this);
  layout->addWidget(m_inputRow);

  connect(m_inputRow, &InputRow::submitted, this, &BaseCard::parseInput);
}

SliderRow *BaseCard::addRow(const QString &label, double min, double max,
                            double step, int decimals) {
  auto *row = new SliderRow(label, min, max, step, decimals, this);
  m_rowsLayout->addWidget(row);
  return row;
}
