#pragma once

#include <QFrame>

class QLabel;

class Status : public QFrame {
  Q_OBJECT

public:
  explicit Status(QWidget *parent = nullptr);

  void setResolution(int w, int h);
  void setPixelInfo(int x, int y, const QColor &c);
  void setStatus(const QString &msg, int type);

private:
  QLabel *m_resLabel{nullptr};
  QLabel *m_pixelLabel{nullptr};
  QLabel *m_statusLabel{nullptr};
};