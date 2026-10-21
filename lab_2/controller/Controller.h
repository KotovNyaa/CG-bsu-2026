#pragma once

#include "../core/Types.h"
#include <QImage>
#include <QObject>

class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(QObject *parent = nullptr);

  QImage currentImage() const { return m_current; }
  QImage originalImage() const { return m_original; }
  HistData histData() const { return m_hist; }

  void load(const QString &path);
  void save(const QString &path);
  void resetOriginal();
  void showOriginal(bool original);

  void setContrastMode(int mode);
  void setContrastRange(int minVal, int maxVal);
  void autoContrast();

  void setFilter(int type, int radius);

  void commit();
  void discard();

signals:
  void imageChanged(const QImage &img);
  void histChanged(const HistData &data);
  void statusChanged(const QString &msg, int type);
  void contrastRangeCalculated(int minVal, int maxVal);
  void stateReset();

private:
  void makeStubImage();
  void updatePipeline();

  QImage m_original;
  QImage m_base;
  QImage m_current;
  HistData m_hist;

  int m_contrastMode{0};
  int m_minVal{0};
  int m_maxVal{255};

  int m_filterType{0};
  int m_radius{1};
};