#include "Controller.h"
#include "../core/Bench.h"
#include "../core/Ops.h"

Controller::Controller(QObject *parent) : QObject(parent) {
    recalc();
}

void Controller::setPrim(Core::Prim p) {
    if (m_prim == p) return;
    m_prim = p;
    m_algo = (p == Core::Prim::Circle) ? Core::Algo::Circle : Core::Algo::Bresenham;
    m_hasSel = false;
    recalc();
}

void Controller::setAlgo(Core::Algo a) {
    if (m_algo == a) return;
    m_algo = a;
    m_hasSel = false;
    recalc();
}

void Controller::setLine(int x1, int y1, int x2, int y2) {
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
    m_hasSel = false;
    recalc();
}

void Controller::setCircle(int xc, int yc, int r) {
    m_xc = xc;
    m_yc = yc;
    m_r = r;
    m_hasSel = false;
    recalc();
}

void Controller::setGrid(bool v) {
    if (m_grid == v) return;
    m_grid = v;
    emit viewChanged();
}

void Controller::setAxes(bool v) {
    if (m_axes == v) return;
    m_axes = v;
    emit viewChanged();
}

void Controller::setIdeal(bool v) {
    if (m_ideal == v) return;
    m_ideal = v;
    emit viewChanged();
}

void Controller::setDots(bool v) {
    if (m_dots == v) return;
    m_dots = v;
    emit viewChanged();
}

void Controller::setLabels(bool v) {
    if (m_labels == v) return;
    m_labels = v;
    emit viewChanged();
}

void Controller::selectStep(int idx) {
    if (idx >= 0 && idx < static_cast<int>(m_res.steps.size())) {
        m_hasSel = true;
        m_selX = m_res.steps[idx].x;
        m_selY = m_res.steps[idx].y;
        emit selChanged(m_selX, m_selY, true);
    }
}

void Controller::clearSel() {
    if (!m_hasSel) return;
    m_hasSel = false;
    emit selChanged(0, 0, false);
}

void Controller::runBench(int runs) {
    m_rep = Core::Bench::run(m_prim, runs);
    emit reportChanged(m_rep);
}

void Controller::recalc() {
    if (m_prim == Core::Prim::Line) {
        switch (m_algo) {
            case Core::Algo::Step: m_res = Core::Ops::step(m_x1, m_y1, m_x2, m_y2, true); break;
            case Core::Algo::Dda: m_res = Core::Ops::dda(m_x1, m_y1, m_x2, m_y2, true); break;
            case Core::Algo::Bresenham: m_res = Core::Ops::bresenham(m_x1, m_y1, m_x2, m_y2, true); break;
            case Core::Algo::Castle: m_res = Core::Ops::castle(m_x1, m_y1, m_x2, m_y2, true); break;
            case Core::Algo::Wu: m_res = Core::Ops::wu(m_x1, m_y1, m_x2, m_y2, true); break;
            default: break;
        }
    } else {
        m_res = Core::Ops::circle(m_xc, m_yc, m_r, true);
    }
    emit resultChanged(m_res);
}