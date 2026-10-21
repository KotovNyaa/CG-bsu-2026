#include "Viewer.h"
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <algorithm>

Viewer::Viewer(QWidget *parent) : QFrame(parent) {
  setObjectName("viewer");
  setMouseTracking(true);
}

void Viewer::setImage(const QImage &img) {
  m_img = img;
  if (!m_initialized) {
    if (width() > 100 && height() > 100) {
      m_initialized = true;
      fit();
    }
  } else {
    update();
  }
}

void Viewer::fit() {
  if (m_img.isNull() || width() <= 20 || height() <= 20) return;
  const double sx = static_cast<double>(width() - 20) / m_img.width();
  const double sy = static_cast<double>(height() - 20) / m_img.height();
  m_zoom = std::min(sx, sy);
  m_offset = QPointF((width() - m_img.width() * m_zoom) * 0.5,
                     (height() - m_img.height() * m_zoom) * 0.5);
  emit zoomChanged(m_zoom);
  update();
}

void Viewer::setZoom100() {
  if (m_img.isNull()) return;
  applyZoom(1.0, QPointF(width() * 0.5, height() * 0.5));
}

void Viewer::zoomIn() {
  applyZoom(m_zoom * 1.2, QPointF(width() * 0.5, height() * 0.5));
}

void Viewer::zoomOut() {
  applyZoom(m_zoom / 1.2, QPointF(width() * 0.5, height() * 0.5));
}

void Viewer::applyZoom(double newZoom, const QPointF &center) {
  if (m_img.isNull()) return;
  newZoom = std::clamp(newZoom, 0.05, 32.0);
  m_offset = center - (center - m_offset) * (newZoom / m_zoom);
  m_zoom = newZoom;
  emit zoomChanged(m_zoom);
  update();
}

void Viewer::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.fillRect(rect(), QColor("#0a0b0d"));

  if (m_img.isNull()) return;

  p.setRenderHint(QPainter::SmoothPixmapTransform, m_zoom < 1.0);
  const QRectF target(m_offset.x(), m_offset.y(),
                      m_img.width() * m_zoom, m_img.height() * m_zoom);
  p.drawImage(target, m_img);

  p.setPen(QColor("#23252b"));
  p.drawRect(target);
}

void Viewer::wheelEvent(QWheelEvent *e) {
  const double factor = e->angleDelta().y() > 0 ? 1.15 : (1.0 / 1.15);
  applyZoom(m_zoom * factor, e->position());
}

void Viewer::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton || e->button() == Qt::MiddleButton) {
    m_dragging = true;
    m_lastMousePos = e->pos();
    setCursor(Qt::ClosedHandCursor);
  }
}

void Viewer::mouseMoveEvent(QMouseEvent *e) {
  if (m_dragging) {
    m_offset += (e->pos() - m_lastMousePos);
    m_lastMousePos = e->pos();
    update();
  }

  if (!m_img.isNull()) {
    const double ix = (e->position().x() - m_offset.x()) / m_zoom;
    const double iy = (e->position().y() - m_offset.y()) / m_zoom;
    if (ix >= 0 && iy >= 0 && ix < m_img.width() && iy < m_img.height()) {
      emit pixelHovered(static_cast<int>(ix), static_cast<int>(iy),
                        m_img.pixelColor(static_cast<int>(ix), static_cast<int>(iy)));
    }
  }
}

void Viewer::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton || e->button() == Qt::MiddleButton) {
    m_dragging = false;
    unsetCursor();
  }
}

void Viewer::showEvent(QShowEvent *) {
  if (!m_initialized && !m_img.isNull()  & width() > 100 && height() > 100) {
    m_initialized = true;
    fit();
  }
}

void Viewer::resizeEvent(QResizeEvent *) {
  if (!m_initialized && width() > 100 && height() > 100 && !m_img.isNull()) {
    m_initialized = true;
    fit();
  }
}