#pragma once

#include "Types.h"
#include <QImage>

class Ops {
public:
  static void computeHist(const QImage &img, HistData &data);
  static QImage linearStretch(const QImage &src, int minVal, int maxVal);
  static QImage equalizeRgb(const QImage &src);
  static QImage equalizeHls(const QImage &src);
  static QImage applyFilter(const QImage &src, int type, int radius);
};