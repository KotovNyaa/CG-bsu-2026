#include "HlsCard.h"
#include <QStringList>

HlsCard::HlsCard(QWidget *parent)
    : BaseCard("HLS (Hue, Lightness, Saturation)", "HLS", parent) {
  m_h = addRow("H", 0, 360, 0.5, 1);
  m_l = addRow("L", 0, 100, 0.5, 1);
  m_s = addRow("S", 0, 100, 0.5, 1);

  inputRow()->setPlaceholder("360, 100, 100");

  connect(m_h, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(0, v, m_h); });
  connect(m_l, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(1, v / 100.0, m_l); });
  connect(m_s, &SliderRow::valueChanged, this,
          [this](double v) { emit channelChanged(2, v / 100.0, m_s); });
}

void HlsCard::setValues(const ColorLib::Hls &hls, QObject *origin) {
  if (origin != m_h)
    m_h->setValue(hls.h);
  if (origin != m_l)
    m_l->setValue(hls.l * 100.0);
  if (origin != m_s)
    m_s->setValue(hls.s * 100.0);
  if (origin != inputRow()) {
    inputRow()->setValue(QString("%1, %2, %3")
                             .arg(hls.h, 0, 'f', 1)
                             .arg(hls.l * 100.0, 0, 'f', 1)
                             .arg(hls.s * 100.0, 0, 'f', 1));
  }
}

void HlsCard::parseInput(const QString &text) {
  const auto parts = text.split(',');
  if (parts.size() == 3) {
    bool hOk, lOk, sOk;
    const double h = parts[0].trimmed().toDouble(&hOk);
    const double l = parts[1].trimmed().toDouble(&lOk);
    const double s = parts[2].trimmed().toDouble(&sOk);
    if (hOk && lOk && sOk)
      emit colorChanged({h, l / 100.0, s / 100.0}, inputRow());
  }
}
