#include "MainWindow.h"
#include "../controller/Controller.h"
#include "widgets/status/Status.h"
#include "widgets/tools/Contrast.h"
#include "widgets/tools/Filter.h"
#include "widgets/tools/Hist.h"
#include "widgets/tools/Tools.h"
#include "widgets/top/Top.h"
#include "widgets/viewer/Viewer.h"
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_controller = new Controller(this);

  build();
  bind();

  installEventFilter(this);

  QFile styleFile(":/ui/style/Stylesheet.qss");
  if (styleFile.open(QFile::ReadOnly | QFile::Text))
    setStyleSheet(styleFile.readAll());

  setMinimumSize(960, 580);
  resize(1120, 700);
  setWindowTitle("Image Processing Studio");

  m_viewer->setImage(m_controller->currentImage());
  m_tools->hist()->setData(m_controller->histData());
  m_status->setResolution(m_controller->currentImage().width(),
                          m_controller->currentImage().height());
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e) {
  if (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(e);
    if (ke->key() == Qt::Key_Space && !ke->isAutoRepeat()) {
      m_controller->showOriginal(true);
      return true;
    }
  } else if (e->type() == QEvent::KeyRelease) {
    auto *ke = static_cast<QKeyEvent *>(e);
    if (ke->key() == Qt::Key_Space && !ke->isAutoRepeat()) {
      m_controller->showOriginal(false);
      return true;
    }
  }
  return QMainWindow::eventFilter(obj, e);
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
  auto *middleLayout = new QHBoxLayout(middle);
  middleLayout->setContentsMargins(0, 0, 0, 0);
  middleLayout->setSpacing(0);

  m_viewer = new Viewer(middle);
  m_tools = new Tools(middle);

  middleLayout->addWidget(m_viewer, 1);
  middleLayout->addWidget(m_tools);

  root->addWidget(middle, 1);

  m_status = new Status(central);
  root->addWidget(m_status);
}

void MainWindow::bind() {
  connect(m_top, &Top::openTriggered, this, [this]() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!path.isEmpty()) m_controller->load(path);
  });

  connect(m_top, &Top::saveTriggered, this, [this]() {
    const QString path = QFileDialog::getSaveFileName(
        this, "Save Image", "output.png", "PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)");
    if (!path.isEmpty()) m_controller->save(path);
  });

  connect(m_top, &Top::resetTriggered, m_controller, &Controller::resetOriginal);
  connect(m_top, &Top::showOriginalToggled, m_controller, &Controller::showOriginal);
  connect(m_top, &Top::fitTriggered, m_viewer, &Viewer::fit);
  connect(m_top, &Top::zoom100Triggered, m_viewer, &Viewer::setZoom100);
  connect(m_top, &Top::zoomInTriggered, m_viewer, &Viewer::zoomIn);
  connect(m_top, &Top::zoomOutTriggered, m_viewer, &Viewer::zoomOut);

  connect(m_viewer, &Viewer::zoomChanged, m_top, &Top::setZoom);
  connect(m_viewer, &Viewer::pixelHovered, m_status, &Status::setPixelInfo);

  connect(m_controller, &Controller::imageChanged, this, [this](const QImage &img) {
    m_viewer->setImage(img);
    m_status->setResolution(img.width(), img.height());
  });

  connect(m_controller, &Controller::histChanged, m_tools->hist(), &Hist::setData);
  connect(m_controller, &Controller::statusChanged, m_status, &Status::setStatus);
  connect(m_controller, &Controller::contrastRangeCalculated,
          m_tools->contrast(), &Contrast::setRange);

  connect(m_controller, &Controller::stateReset, this, [this]() {
    m_tools->contrast()->resetState();
    m_tools->filter()->resetState();
  });

  connect(m_tools->contrast(), &Contrast::modeChanged,
          m_controller, &Controller::setContrastMode);
  connect(m_tools->contrast(), &Contrast::rangeChanged,
          m_controller, &Controller::setContrastRange);
  connect(m_tools->contrast(), &Contrast::autoRequested,
          m_controller, &Controller::autoContrast);

  connect(m_tools->filter(), &Filter::filterChanged,
          m_controller, &Controller::setFilter);

  connect(m_tools, &Tools::commitRequested, m_controller, &Controller::commit);
  connect(m_tools, &Tools::discardRequested, m_controller, &Controller::discard);
}