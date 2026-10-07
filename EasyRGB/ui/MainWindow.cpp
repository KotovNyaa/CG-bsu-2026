#include "MainWindow.h"
#include "../controller/Controller.h"
#include "widgets/models/CmykCard.h"
#include "widgets/models/HlsCard.h"
#include "widgets/models/RgbCard.h"
#include "widgets/top/TopPanel.h"
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_controller = new Controller(this);

  buildUi();
  bindController();

  QFile styleFile(":/ui/style/Stylesheet.qss");
  if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
    setStyleSheet(styleFile.readAll());
  }

  setMinimumSize(1052, 672);
  resize(1068, 688);
  setWindowTitle("Color Models Converter");
}

void MainWindow::buildUi() {
  auto *central = new QWidget(this);
  setCentralWidget(central);

  auto *rootLayout = new QGridLayout(central);
  rootLayout->setContentsMargins(0, 0, 0, 0);

  auto *stage = new QWidget(central);
  stage->setObjectName("stage");
  stage->setFixedSize(1020, 640);
  stage->setFocusPolicy(Qt::StrongFocus);

  auto *stageLayout = new QVBoxLayout(stage);
  stageLayout->setContentsMargins(0, 0, 0, 0);
  stageLayout->setSpacing(12);

  m_topPanel = new TopPanel(stage);

  auto *bottomPanel = new QWidget(stage);
  bottomPanel->setFixedHeight(242);

  auto *bottomLayout = new QHBoxLayout(bottomPanel);
  bottomLayout->setContentsMargins(0, 0, 0, 0);
  bottomLayout->setSpacing(12);

  m_rgbCard = new RgbCard(bottomPanel);
  m_cmykCard = new CmykCard(bottomPanel);
  m_hlsCard = new HlsCard(bottomPanel);

  bottomLayout->addWidget(m_rgbCard);
  bottomLayout->addWidget(m_cmykCard);
  bottomLayout->addWidget(m_hlsCard);

  stageLayout->addWidget(m_topPanel);
  stageLayout->addWidget(bottomPanel);

  rootLayout->addWidget(stage, 0, 0, Qt::AlignCenter);

  stage->setFocus();
}

void MainWindow::bindController() {
  connect(m_topPanel, &TopPanel::hsvChanged, this,
          [this](double h, double s, double v, QObject *origin) {
            m_controller->setHsv(h, s, v, origin);
          });
  connect(m_topPanel, &TopPanel::hexSubmitted, this,
          [this](const QString &hex, QObject *origin) {
            m_controller->setHex(hex, origin);
          });

  connect(m_rgbCard, &RgbCard::channelChanged, this,
          [this](int ch, double val, QObject *origin) {
            m_controller->setRgbChannel(ch, val, origin);
          });
  connect(m_rgbCard, &RgbCard::colorChanged, this,
          [this](const ColorLib::Rgb &c, QObject *origin) {
            m_controller->setRgb(c, origin);
          });

  connect(m_cmykCard, &CmykCard::channelChanged, this,
          [this](int ch, double val, QObject *origin) {
            m_controller->setCmykChannel(ch, val, origin);
          });
  connect(m_cmykCard, &CmykCard::colorChanged, this,
          [this](const ColorLib::Cmyk &c, QObject *origin) {
            m_controller->setCmyk(c, origin);
          });

  connect(m_hlsCard, &HlsCard::channelChanged, this,
          [this](int ch, double val, QObject *origin) {
            m_controller->setHlsChannel(ch, val, origin);
          });
  connect(m_hlsCard, &HlsCard::colorChanged, this,
          [this](const ColorLib::Hls &c, QObject *origin) {
            m_controller->setHls(c, origin);
          });

  connect(m_controller, &Controller::rgbUpdated, this,
          [this](const ColorLib::Rgb &c, QObject *origin) {
            m_topPanel->setRgb(c);
            m_rgbCard->setValues(c, origin);
          });

  connect(m_controller, &Controller::hexUpdated, this,
          [this](const QString &hex, QObject *origin) {
            m_topPanel->setHex(hex, origin);
          });

  connect(m_controller, &Controller::hsvUpdated, this,
          [this](double h, double s, double v, QObject *origin) {
            m_topPanel->setHsv(h, s, v, origin);
          });

  connect(m_controller, &Controller::cmykUpdated, this,
          [this](const ColorLib::Cmyk &c, QObject *origin) {
            m_cmykCard->setValues(c, origin);
          });

  connect(m_controller, &Controller::hlsUpdated, this,
          [this](const ColorLib::Hls &c, QObject *origin) {
            m_hlsCard->setValues(c, origin);
          });

  connect(m_controller, &Controller::reportUpdated, m_topPanel,
          &TopPanel::setReport);

  m_controller->setRgb(m_controller->rgb(), nullptr);
}
