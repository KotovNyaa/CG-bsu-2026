#include "CmykCard.h"
#include <QStringList>

CmykCard::CmykCard(QWidget *parent)
    : BaseCard("CMYK (Cyan, Magenta, Yellow, Key)", "CMYK", parent) {
  m_c = addRow("C", 0, 100, 0.1, 1);
  m_m = addRow("M", 0, 100, 0.1, 1);
  m_y = addRow("Y", 0, 100, 0.1, 1);
  m_k = addRow("K", 0, 100, 0.1, 1);

  inputRow()->setPlaceholder("0, 0, 0, 100");

  connect(m_c, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(0, v / 100.0, m_c); });
  connect(m_m, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(1, v / 100.0, m_m); });
  connect(m_y, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(2, v / 100.0, m_y); });
  connect(m_k, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(3, v / 100.0, m_k); });
}

void CmykCard::setValues(const ColorLib::Cmyk &cmyk, QObject *origin) {
  if (origin != m_c)
    m_c->setValue(cmyk.c * 100.0);
  if (origin != m_m)
    m_m->setValue(cmyk.m * 100.0);
  if (origin != m_y)
    m_y->setValue(cmyk.y * 100.0);
  if (origin != m_k)
    m_k->setValue(cmyk.k * 100.0);
  if (origin != inputRow()) {
    inputRow()->setValue(QString("%1, %2, %3, %4")
                             .arg(cmyk.c * 100.0, 0, 'f', 1)
                             .arg(cmyk.m * 100.0, 0, 'f', 1)
                             .arg(cmyk.y * 100.0, 0, 'f', 1)
                             .arg(cmyk.k * 100.0, 0, 'f', 1));
  }
}

void CmykCard::parseInput(const QString &text) {
  const auto parts = text.split(',');
  if (parts.size() == 4) {
    bool cOk, mOk, yOk, kOk;
    const double c = parts[0].trimmed().toDouble(&cOk);
    const double m = parts[1].trimmed().toDouble(&mOk);
    const double y = parts[2].trimmed().toDouble(&yOk);
    const double k = parts[3].trimmed().toDouble(&kOk);
    if (cOk && mOk && yOk && kOk)
      emit colorChanged({c / 100.0, m / 100.0, y / 100.0, k / 100.0},
                        inputRow());
  }
}
