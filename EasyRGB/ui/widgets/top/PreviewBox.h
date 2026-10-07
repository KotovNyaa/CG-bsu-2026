#pragma once

#include <QColor>
#include <QWidget>

class PreviewBox : public QWidget {
  Q_OBJECT

public:
  explicit PreviewBox(QWidget *parent = nullptr);
  void setColor(const QColor &color);

protected:
  void paintEvent(QPaintEvent *) override;

private:
  QColor m_color{123, 123, 4};
};