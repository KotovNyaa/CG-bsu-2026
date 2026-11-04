#pragma once

#include <QMainWindow>

class Controller;
class Top;
class Canvas;
class Side;
class Status;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void build();
  void bind();

  Controller *m_controller{nullptr};
  Top *m_top{nullptr};
  Canvas *m_canvas{nullptr};
  Side *m_side{nullptr};
  Status *m_status{nullptr};
};
