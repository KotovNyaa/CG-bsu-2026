#pragma once

#include "../../../core/Types.h"
#include <QWidget>

class Combo;
class QLabel;

class HistCanvas : public QWidget {
  Q_OBJECT

public:
  explicit HistCanvas(QWidget *parent = nullptr);
  void setData(const HistData &data);
  void setMode(int mode);

protected:
  void paintEvent(QPaintEvent *) override;

private:
  HistData m_data;
  int m_mode{0};
};

class Hist : public QWidget {
  Q_OBJECT

public:
  explicit Hist(QWidget *parent = nullptr);
  void setData(const HistData &data);

private:
  HistCanvas *m_canvas{nullptr};
  QLabel *m_stats{nullptr};
};