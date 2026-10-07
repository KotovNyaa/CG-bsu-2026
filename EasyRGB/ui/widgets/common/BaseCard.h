#pragma once

#include "InputRow.h"
#include "SliderRow.h"
#include <QFrame>
#include <QVBoxLayout>

class BaseCard : public QFrame {
  Q_OBJECT

public:
  explicit BaseCard(const QString &title, const QString &inputLabel,
                    QWidget *parent = nullptr);

  InputRow *inputRow() const { return m_inputRow; }

protected:
  SliderRow *addRow(const QString &label, double min, double max,
                    double step = 1.0, int decimals = 0);
  virtual void parseInput(const QString &text) = 0;

private:
  QVBoxLayout *m_rowsLayout{nullptr};
  InputRow *m_inputRow{nullptr};
};
