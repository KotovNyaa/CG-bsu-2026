#include "Canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>

Canvas::Canvas(QWidget *parent) : QFrame(parent) {
    setObjectName("canvas");
    setMouseTracking(true);
}

void Canvas::setResult(const Core::Result &res) {
    m_res = res;
    update();
}

void Canvas::setLine(int x1, int y1, int x2, int y2) {
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
    update();
}

void Canvas::setCircle(int xc, int yc, int r) {
    m_xc = xc;
    m_yc = yc;
    m_r = r;
    update();
}

void Canvas::setGrid(bool v) { m_grid = v; update(); }
void Canvas::setAxes(bool v) { m_axes = v; update(); }
void Canvas::setIdeal(bool v) { m_ideal = v; update(); }
void Canvas::setDots(bool v) { m_dots = v; update(); }
void Canvas::setLabels(bool v) { m_labels = v; update(); }

void Canvas::setSel(int x, int y, bool has) {
    m_hasSel = has;
    m_selX = x;
    m_selY = y;
    update();
}

void Canvas::reset() {
    m_offset = QPointF(width() * 0.5, height() * 0.5);
    m_zoom = 24.0;
    emit zoomChanged(m_zoom);
    update();
}

void Canvas::zoom100() {
    applyZoom(24.0, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::zoomIn() {
    applyZoom(m_zoom * 1.25, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::zoomOut() {
    applyZoom(m_zoom / 1.25, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::applyZoom(double nz, const QPointF &center) {
    nz = std::clamp(nz, 2.0, 150.0);
    m_offset = center - (center - m_offset) * (nz / m_zoom);
    m_zoom = nz;
    emit zoomChanged(m_zoom);
    update();
}

QPointF Canvas::toScreen(double gx, double gy) const {
    return {m_offset.x() + gx * m_zoom, m_offset.y() - gy * m_zoom};
}

QPoint Canvas::fromScreen(const QPointF &sp) const {
    const int gx = static_cast<int>(std::floor((sp.x() - m_offset.x()) / m_zoom));
    const int gy = static_cast<int>(std::floor((m_offset.y() - sp.y()) / m_zoom));
    return {gx, gy};
}

void Canvas::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor("#0a0b0d"));

    const int minX = static_cast<int>(std::floor(-m_offset.x() / m_zoom)) - 1;
    const int maxX = static_cast<int>(std::ceil((width() - m_offset.x()) / m_zoom)) + 1;
    const int minY = static_cast<int>(std::floor((m_offset.y() - height()) / m_zoom)) - 1;
    const int maxY = static_cast<int>(std::ceil(m_offset.y() / m_zoom)) + 1;

    if (m_grid && m_zoom >= 4.0) {
        p.setPen(QPen(QColor("#18191f"), 1));
        for (int x = minX; x <= maxX; ++x) {
            const double sx = m_offset.x() + x * m_zoom;
            p.drawLine(QPointF(sx, 0), QPointF(sx, height()));
        }
        for (int y = minY; y <= maxY; ++y) {
            const double sy = m_offset.y() - y * m_zoom;
            p.drawLine(QPointF(0, sy), QPointF(width(), sy));
        }
    }

    p.setPen(Qt::NoPen);
    for (const auto &px : m_res.pixels) {
        const double sx = m_offset.x() + px.x * m_zoom;
        const double sy = m_offset.y() - (px.y + 1) * m_zoom;
        p.setBrush(QColor(59, 130, 246, static_cast<int>(255 * px.a)));
        p.drawRect(QRectF(sx, sy, m_zoom, m_zoom));

        if (m_dots && m_zoom >= 10.0) {
            p.setBrush(QColor("#93c5fd"));
            p.drawEllipse(QPointF(sx + m_zoom * 0.5, sy + m_zoom * 0.5), 1.5, 1.5);
        }

        if (m_labels && m_zoom >= 36.0) {
            p.setPen(QColor("#e2e8f0"));
            p.setFont(QFont("monospace", 7));
            p.drawText(QRectF(sx, sy, m_zoom, m_zoom), Qt::AlignCenter,
                       QString("%1,%2").arg(px.x).arg(px.y));
            p.setPen(Qt::NoPen);
        }
    }

    if (m_hasSel) {
        const double sx = m_offset.x() + m_selX * m_zoom;
        const double sy = m_offset.y() - (m_selY + 1) * m_zoom;
        p.setBrush(QColor(250, 204, 21, 180));
        p.setPen(QPen(QColor("#fef08a"), 2));
        p.drawRect(QRectF(sx, sy, m_zoom, m_zoom));
    }

    if (m_hasHover) {
        const double hx = m_offset.x() + m_hoverX * m_zoom;
        const double hy = m_offset.y() - (m_hoverY + 1) * m_zoom;
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(96, 165, 250, 100), 1));
        p.drawRect(QRectF(hx, hy, m_zoom, m_zoom));
    }

    if (m_axes) {
        const double ox = m_offset.x();
        const double oy = m_offset.y();

        p.setPen(QPen(QColor("#4b5563"), 1.5));
        p.drawLine(QPointF(0, oy), QPointF(width(), oy));
        p.drawLine(QPointF(ox, 0), QPointF(ox, height()));

        p.setBrush(QColor("#9ca3af"));
        const QPointF ax[3] = {{static_cast<double>(width()) - 8, oy - 4},
                               {static_cast<double>(width()), oy},
                               {static_cast<double>(width()) - 8, oy + 4}};
        p.drawPolygon(ax, 3);
        const QPointF ay[3] = {{ox - 4, 8}, {ox, 0}, {ox + 4, 8}};
        p.drawPolygon(ay, 3);

        int step = 1;
        if (m_zoom < 10.0) step = 10;
        else if (m_zoom < 20.0) step = 5;
        else if (m_zoom < 35.0) step = 2;

        p.setFont(QFont("monospace", 8));
        p.setPen(QColor("#6b7280"));

        for (int x = (minX / step) * step; x <= maxX; x += step) {
            if (x == 0) continue;
            const double sx = ox + x * m_zoom;
            p.drawLine(QPointF(sx, oy - 3), QPointF(sx, oy + 3));
            p.drawText(QRectF(sx - 20, oy + 4, 40, 14), Qt::AlignCenter, QString::number(x));
        }

        for (int y = (minY / step) * step; y <= maxY; y += step) {
            if (y == 0) continue;
            const double sy = oy - y * m_zoom;
            p.drawLine(QPointF(ox - 3, sy), QPointF(ox + 3, sy));
            p.drawText(QRectF(ox + 5, sy - 7, 30, 14), Qt::AlignLeft | Qt::AlignVCenter, QString::number(y));
        }
        p.drawText(QRectF(ox + 4, oy + 4, 15, 14), Qt::AlignLeft, "0");
    }

    if (m_ideal) {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(244, 63, 94, 200), 1.5, Qt::DashLine));
        if (m_res.prim == Core::Prim::Line) {
            const QPointF p1(m_offset.x() + (m_x1 + 0.5) * m_zoom,
                             m_offset.y() - (m_y1 + 0.5) * m_zoom);
            const QPointF p2(m_offset.x() + (m_x2 + 0.5) * m_zoom,
                             m_offset.y() - (m_y2 + 0.5) * m_zoom);
            p.drawLine(p1, p2);
        } else {
            const QPointF center(m_offset.x() + (m_xc + 0.5) * m_zoom,
                                 m_offset.y() - (m_yc + 0.5) * m_zoom);
            const double r = m_r * m_zoom;
            p.drawEllipse(center, r, r);
        }
    }
}

void Canvas::wheelEvent(QWheelEvent *e) {
    const double factor = e->angleDelta().y() > 0 ? 1.15 : (1.0 / 1.15);
    applyZoom(m_zoom * factor, e->position());
}

void Canvas::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton || e->button() == Qt::MiddleButton) {
        m_drag = true;
        m_lastPos = e->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *e) {
    if (m_drag) {
        m_offset += (e->pos() - m_lastPos);
        m_lastPos = e->pos();
        update();
    }
    m_hasHover = true;
    const auto g = fromScreen(e->position());
    if (g.x() != m_hoverX || g.y() != m_hoverY) {
        m_hoverX = g.x();
        m_hoverY = g.y();
        emit hovered(m_hoverX, m_hoverY);
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton || e->button() == Qt::MiddleButton) {
        m_drag = false;
        unsetCursor();
    }
}

void Canvas::resizeEvent(QResizeEvent *) {
    if (!m_init && width() > 50 && height() > 50) {
        m_init = true;
        reset();
    }
}
void Canvas::leaveEvent(QEvent *) {
    if (m_hasHover) {
        m_hasHover = false;
        update();
}}