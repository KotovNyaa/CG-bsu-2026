#pragma once

#include "../../../lib/ColorTypes.h"
#include "../common/BaseCard.h"

class HlsCard : public BaseCard {
  Q_OBJECT

public:
  explicit HlsCard(QWidget *parent = nullptr);
  void setValues(const ColorLib::Hls &hls, QObject *origin);

signals:
  void channelChanged(int ch, double val, QObject *origin);
  void colorChanged(const ColorLib::Hls &hls, QObject *origin);

protected:
  void parseInput(const QString &text) override;

private:
  SliderRow *m_h{nullptr};
  SliderRow *m_l{nullptr};
  SliderRow *m_s{nullptr};
};
