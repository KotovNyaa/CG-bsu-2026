#pragma once

#include "../../../lib/ColorTypes.h"
#include "../common/BaseCard.h"

class RgbCard : public BaseCard {
  Q_OBJECT

public:
  explicit RgbCard(QWidget *parent = nullptr);
  void setValues(const ColorLib::Rgb &rgb, QObject *origin);

signals:
  void channelChanged(int ch, double val, QObject *origin);
  void colorChanged(const ColorLib::Rgb &rgb, QObject *origin);

protected:
  void parseInput(const QString &text) override;

private:
  SliderRow *m_r{nullptr};
  SliderRow *m_g{nullptr};
  SliderRow *m_b{nullptr};
};
