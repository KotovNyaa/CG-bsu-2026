#pragma once

#include <QFrame>

class QPushButton;
class QLabel;

class Top : public QFrame {
  Q_OBJECT

public:
  explicit Top(QWidget *parent = nullptr);
  void setZoom(double z);

signals:
  void openTriggered();
  void saveTriggered();
  void resetTriggered();
  void showOriginalToggled(bool original);
  void fitTriggered();
  void zoom100Triggered();
  void zoomInTriggered();
  void zoomOutTriggered();

private:
  QPushButton *m_origBtn{nullptr};
  QLabel *m_zoomLabel{nullptr};
};