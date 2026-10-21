#pragma once

#include <QMainWindow>

class Controller;
class Top;
class Viewer;
class Tools;
class Status;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  bool eventFilter(QObject *obj, QEvent *e) override;

private:
  void build();
  void bind();

  Controller *m_controller{nullptr};
  Top *m_top{nullptr};
  Viewer *m_viewer{nullptr};
  Tools *m_tools{nullptr};
  Status *m_status{nullptr};
};