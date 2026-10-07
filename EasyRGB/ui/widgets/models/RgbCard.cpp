#include "RgbCard.h"
#include <QStringList>
#include <cmath>

RgbCard::RgbCard(QWidget *parent)
    : BaseCard("RGB (Red, Green, Blue)", "RGB", parent) {
  m_r = addRow("R", 0, 255);
  m_g = addRow("G", 0, 255);
  m_b = addRow("B", 0, 255);

  inputRow()->setPlaceholder("255, 255, 255");

  connect(m_r, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(0, v / 255.0, m_r); });
  connect(m_g, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(1, v / 255.0, m_g); });
  connect(m_b, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(2, v / 255.0, m_b); });
}

void RgbCard::setValues(const ColorLib::Rgb &rgb, QObject *origin) {
  if (origin != m_r)
    m_r->setValue(rgb.r * 255.0);
  if (origin != m_g)
    m_g->setValue(rgb.g * 255.0);
  if (origin != m_b)
    m_b->setValue(rgb.b * 255.0);
  if (origin != inputRow()) {
    inputRow()->setValue(QString("%1, %2, %3")
                             .arg(static_cast<int>(std::round(rgb.r * 255.0)))
                             .arg(static_cast<int>(std::round(rgb.g * 255.0)))
                             .arg(static_cast<int>(std::round(rgb.b * 255.0))));
  }
}

void RgbCard::parseInput(const QString &text) {
  const auto parts = text.split(',');
  if (parts.size() == 3) {
    bool rOk, gOk, bOk;
    const double r = parts[0].trimmed().toDouble(&rOk);
    const double g = parts[1].trimmed().toDouble(&gOk);
    const double b = parts[2].trimmed().toDouble(&bOk);
    if (rOk && gOk && bOk)
      emit colorChanged({r / 255.0, g / 255.0, b / 255.0}, inputRow());
  }
}
