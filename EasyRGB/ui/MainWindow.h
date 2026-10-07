#pragma once

#include <QMainWindow>

class Controller;
class TopPanel;
class RgbCard;
class CmykCard;
class HlsCard;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void buildUi();
  void bindController();

  Controller *m_controller{nullptr};
  TopPanel *m_topPanel{nullptr};
  RgbCard *m_rgbCard{nullptr};
  CmykCard *m_cmykCard{nullptr};
  HlsCard *m_hlsCard{nullptr};
};