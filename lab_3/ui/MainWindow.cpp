#include "MainWindow.h"
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "../controller/Controller.h"
#include "widgets/canvas/Canvas.h"
#include "widgets/side/Side.h"
#include "widgets/status/Status.h"
#include "widgets/top/Top.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_controller = new Controller(this);

    build();
    bind();

    QFile qss(":/ui/style/Stylesheet.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text)) {
        setStyleSheet(qss.readAll());
    }

    setMinimumSize(960, 600);
    resize(1140, 720);
    setWindowTitle("Raster Algorithms Studio");

    m_canvas->setResult(m_controller->result());
    m_canvas->setLine(m_controller->x1(), m_controller->y1(), m_controller->x2(), m_controller->y2());
    m_canvas->setCircle(m_controller->xc(), m_controller->yc(), m_controller->r());
    m_side->setResult(m_controller->result());
    m_status->setStats(static_cast<int>(m_controller->result().pixels.size()), m_controller->result().time);
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
    connect(m_top, &Top::zoomIn, m_canvas, &Canvas::zoomIn);
    connect(m_top, &Top::zoomOut, m_canvas, &Canvas::zoomOut);
    connect(m_top, &Top::zoom100, m_canvas, &Canvas::zoom100);
    connect(m_top, &Top::reset, m_canvas, &Canvas::reset);

    connect(m_top, &Top::gridToggled, m_controller, &Controller::setGrid);
    connect(m_top, &Top::axesToggled, m_controller, &Controller::setAxes);
    connect(m_top, &Top::idealToggled, m_controller, &Controller::setIdeal);
    connect(m_top, &Top::dotsToggled, m_controller, &Controller::setDots);
    connect(m_top, &Top::labelsToggled, m_controller, &Controller::setLabels);

    connect(m_canvas, &Canvas::zoomChanged, m_top, &Top::setZoom);
    connect(m_canvas, &Canvas::hovered, m_status, &Status::setCoord);

    connect(m_side, &Side::primChanged, m_controller, &Controller::setPrim);
    connect(m_side, &Side::algoChanged, m_controller, &Controller::setAlgo);
    connect(m_side, &Side::lineChanged, m_controller, &Controller::setLine);
    connect(m_side, &Side::circleChanged, m_controller, &Controller::setCircle);
    connect(m_side, &Side::stepSelected, m_controller, &Controller::selectStep);
    connect(m_side, &Side::benchRequested, m_controller, &Controller::runBench);

    connect(m_controller, &Controller::resultChanged, this, [this](const Core::Result &res) {
        m_canvas->setResult(res);
        m_canvas->setLine(m_controller->x1(), m_controller->y1(), m_controller->x2(), m_controller->y2());
        m_canvas->setCircle(m_controller->xc(), m_controller->yc(), m_controller->r());
        m_side->setResult(res);
        m_status->setStats(static_cast<int>(res.pixels.size()), res.time);
    });

    connect(m_controller, &Controller::reportChanged, m_side, &Side::setReport);

    connect(m_controller, &Controller::viewChanged, this, [this]() {
        m_canvas->setGrid(m_controller->grid());
        m_canvas->setAxes(m_controller->axes());
        m_canvas->setIdeal(m_controller->ideal());
        m_canvas->setDots(m_controller->dots());
        m_canvas->setLabels(m_controller->labels());
    });

    connect(m_controller, &Controller::selChanged, m_canvas, &Canvas::setSel);
}