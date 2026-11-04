#include "MainWindow.h"
#include "../controller/Controller.h"
#include "widgets/canvas/Canvas.h"
#include "widgets/side/Side.h"
#include "widgets/status/Status.h"
#include "widgets/top/Top.h"
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_controller = new Controller(this);

  build();
  bind();

  QFile qss(":/ui/style/Stylesheet.qss");
  if (qss.open(QFile::ReadOnly | QFile::Text)) {
    setStyleSheet(qss.readAll());
  }

  setMinimumSize(980, 620);
  resize(1180, 740);
  setWindowTitle("Clipping Algorithms Studio");

  m_side->syncState(m_controller->mode(), m_controller->window(),
                    m_controller->geometryText());
  const auto &res = m_controller->result();
  m_canvas->setResult(res);
  m_side->setResult(res);
  m_status->setStats(res.mode, res.total, res.visible, res.time, res.convex);
}

void MainWindow::build() {
  auto *central = new QWidget(this);
  setCentralWidget(central);

  auto *root = new QVBoxLayout(central);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  m_top = new Top(central);
  root->addWidget(m_top);

  auto *middle = new QWidget(central);
  auto *midLay = new QHBoxLayout(middle);
  midLay->setContentsMargins(0, 0, 0, 0);
  midLay->setSpacing(0);

  m_canvas = new Canvas(middle);
  m_side = new Side(middle);

  midLay->addWidget(m_canvas, 1);
  midLay->addWidget(m_side);
  root->addWidget(middle, 1);

  m_status = new Status(central);
  root->addWidget(m_status);
}

void MainWindow::bind() {
  connect(m_top, &Top::openTriggered, this, [this]() {
    const QString path =
        QFileDialog::getOpenFileName(this, "Open Clipping Data", "",
                                     "Text Files (*.txt *.dat);;All Files (*)");
    if (!path.isEmpty())
      m_controller->load(path);
  });

  connect(m_top, &Top::saveTriggered, this, [this]() {
    const QString path = QFileDialog::getSaveFileName(
        this, "Save Clipping Data", "clipping_data.txt",
        "Text Files (*.txt);;All Files (*)");
    if (!path.isEmpty())
      m_controller->save(path);
  });

  connect(m_top, &Top::zoomIn, m_canvas, &Canvas::zoomIn);
  connect(m_top, &Top::zoomOut, m_canvas, &Canvas::zoomOut);
  connect(m_top, &Top::fit, m_canvas, &Canvas::fit);
  connect(m_top, &Top::zoom100, m_canvas, &Canvas::zoom100);
  connect(m_top, &Top::reset, m_canvas, &Canvas::reset);

  connect(m_top, &Top::gridToggled, m_controller, &Controller::setGrid);
  connect(m_top, &Top::axesToggled, m_controller, &Controller::setAxes);
  connect(m_top, &Top::winToggled, m_controller, &Controller::setWinLayer);
  connect(m_top, &Top::origToggled, m_controller, &Controller::setOrigLayer);
  connect(m_top, &Top::clipToggled, m_controller, &Controller::setClipLayer);
  connect(m_top, &Top::labelsToggled, m_controller, &Controller::setLabels);

  connect(m_canvas, &Canvas::zoomChanged, m_top, &Top::setZoom);
  connect(m_canvas, &Canvas::hovered, m_status, &Status::setCoord);

  connect(m_side, &Side::modeChanged, m_controller, &Controller::setMode);
  connect(m_side, &Side::windowChanged, m_controller, &Controller::setWindow);
  connect(m_side, &Side::geometryChanged, m_controller,
          &Controller::setGeometryText);
  connect(m_side, &Side::presetRequested, m_controller,
          &Controller::applyPreset);
  connect(m_side, &Side::stepSelected, m_controller, &Controller::selectStep);

  connect(m_controller, &Controller::resultChanged, this,
          [this](const Core::Result &res) {
            m_canvas->setResult(res);
            m_side->setResult(res);
            m_status->setStats(res.mode, res.total, res.visible, res.time,
                               res.convex);
          });

  connect(m_controller, &Controller::stateSynced, this, [this]() {
    m_side->syncState(m_controller->mode(), m_controller->window(),
                      m_controller->geometryText());
  });

  connect(m_controller, &Controller::viewChanged, this, [this]() {
    m_canvas->setGrid(m_controller->grid());
    m_canvas->setAxes(m_controller->axes());
    m_canvas->setWinLayer(m_controller->winLayer());
    m_canvas->setOrigLayer(m_controller->origLayer());
    m_canvas->setClipLayer(m_controller->clipLayer());
    m_canvas->setLabels(m_controller->labels());
  });

  connect(m_controller, &Controller::fitRequested, m_canvas, &Canvas::fit);
  connect(m_controller, &Controller::selChanged, m_canvas, &Canvas::setSel);
  connect(m_controller, &Controller::statusChanged, m_status,
          &Status::setStatus);
}
