#pragma once

#include "../../../lib/ColorTypes.h"
#include "../common/BaseCard.h"

class CmykCard : public BaseCard {
  Q_OBJECT

public:
  explicit CmykCard(QWidget *parent = nullptr);
  void setValues(const ColorLib::Cmyk &cmyk, QObject *origin);

signals:
  void channelChanged(int ch, double val, QObject *origin);
  void colorChanged(const ColorLib::Cmyk &cmyk, QObject *origin);

protected:
  void parseInput(const QString &text) override;

private:
  SliderRow *m_c{nullptr};
  SliderRow *m_m{nullptr};
  SliderRow *m_y{nullptr};
  SliderRow *m_k{nullptr};
};
