#include "Canvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
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

void Canvas::setGrid(bool v) {
  m_grid = v;
  update();
}
void Canvas::setAxes(bool v) {
  m_axes = v;
  update();
}
void Canvas::setWinLayer(bool v) {
  m_winLayer = v;
  update();
}
void Canvas::setOrigLayer(bool v) {
  m_origLayer = v;
  update();
}
void Canvas::setClipLayer(bool v) {
  m_clipLayer = v;
  update();
}
void Canvas::setLabels(bool v) {
  m_labels = v;
  update();
}

void Canvas::setSel(double x, double y, bool has) {
  m_hasSel = has;
  m_selX = x;
  m_selY = y;
  update();
}

void Canvas::fit() {
  if (width() <= 50 || height() <= 50)
    return;

  const Core::Rect w = m_res.win.normalized();
  double minX = std::min(0.0, w.xmin);
  double maxX = std::max(0.0, w.xmax);
  double minY = std::min(0.0, w.ymin);
  double maxY = std::max(0.0, w.ymax);

  auto expand = [&](double x, double y) {
    minX = std::min(minX, x);
    maxX = std::max(maxX, x);
    minY = std::min(minY, y);
    maxY = std::max(maxY, y);
  };

  if (m_res.mode == Core::Mode::LiangBarsky) {
    for (const auto &s : m_res.segs) {
      expand(s.orig.p1.x, s.orig.p1.y);
      expand(s.orig.p2.x, s.orig.p2.y);
    }
  } else {
    for (const auto &pt : m_res.origPoly) {
      expand(pt.x, pt.y);
    }
  }

  const double spanX = std::max(4.0, (maxX - minX) * 1.35);
  const double spanY = std::max(4.0, (maxY - minY) * 1.35);
  const double midX = (minX + maxX) * 0.5;
  const double midY = (minY + maxY) * 0.5;

  m_zoom = std::clamp(std::min(width() / spanX, height() / spanY), 0.5, 500.0);
  m_offset =
      QPointF(width() * 0.5 - midX * m_zoom, height() * 0.5 + midY * m_zoom);
  emit zoomChanged(m_zoom);
  update();
}

void Canvas::reset() {
  m_offset = QPointF(width() * 0.5, height() * 0.5);
  m_zoom = 48.0;
  emit zoomChanged(m_zoom);
  update();
}

void Canvas::zoom100() {
  applyZoom(48.0, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::zoomIn() {
  applyZoom(m_zoom * 1.25, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::zoomOut() {
  applyZoom(m_zoom / 1.25, QPointF(width() * 0.5, height() * 0.5));
}

void Canvas::applyZoom(double nz, const QPointF &center) {
  nz = std::clamp(nz, 0.5, 500.0);
  m_offset = center - (center - m_offset) * (nz / m_zoom);
  m_zoom = nz;
  emit zoomChanged(m_zoom);
  update();
}

QPointF Canvas::toScreen(double wx, double wy) const {
  return {m_offset.x() + wx * m_zoom, m_offset.y() - wy * m_zoom};
}

QPointF Canvas::toWorld(const QPointF &sp) const {
  return {(sp.x() - m_offset.x()) / m_zoom, (m_offset.y() - sp.y()) / m_zoom};
}

double Canvas::calcStep() const {
  const double raw = 64.0 / m_zoom;
  const double exp = std::pow(10.0, std::floor(std::log10(raw)));
  const double frac = raw / exp;
  if (frac < 1.5)
    return 1.0 * exp;
  if (frac < 3.5)
    return 2.0 * exp;
  if (frac < 7.5)
    return 5.0 * exp;
  return 10.0 * exp;
}

QString Canvas::fmtVal(double v) {
  if (std::abs(v) < 1e-9)
    v = 0.0;
  QString s = QString::number(v, 'f', 2);
  while (s.contains('.') && (s.endsWith('0') || s.endsWith('.'))) {
    if (s.endsWith('.')) {
      s.chop(1);
      break;
    }
    s.chop(1);
  }
  return s;
}

void Canvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.fillRect(rect(), QColor("#0a0b0d"));

  const double step = calcStep();
  const double subStep = step * 0.2;

  const double wMinX = -m_offset.x() / m_zoom;
  const double wMaxX = (width() - m_offset.x()) / m_zoom;
  const double wMinY = (m_offset.y() - height()) / m_zoom;
  const double wMaxY = m_offset.y() / m_zoom;

  if (m_grid) {
    if (subStep * m_zoom >= 8.0) {
      p.setPen(QPen(QColor("#121318"), 1));
      const double startX = std::floor(wMinX / subStep) * subStep;
      for (double x = startX; x <= wMaxX; x += subStep) {
        const double sx = m_offset.x() + x * m_zoom;
        p.drawLine(QPointF(sx, 0), QPointF(sx, height()));
      }
      const double startY = std::floor(wMinY / subStep) * subStep;
      for (double y = startY; y <= wMaxY; y += subStep) {
        const double sy = m_offset.y() - y * m_zoom;
        p.drawLine(QPointF(0, sy), QPointF(width(), sy));
      }
    }

    p.setPen(QPen(QColor("#1a1c23"), 1));
    const double startX = std::floor(wMinX / step) * step;
    for (double x = startX; x <= wMaxX; x += step) {
      const double sx = m_offset.x() + x * m_zoom;
      p.drawLine(QPointF(sx, 0), QPointF(sx, height()));
    }
    const double startY = std::floor(wMinY / step) * step;
    for (double y = startY; y <= wMaxY; y += step) {
      const double sy = m_offset.y() - y * m_zoom;
      p.drawLine(QPointF(0, sy), QPointF(width(), sy));
    }
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

    p.setFont(QFont("monospace", 8));
    p.setPen(QColor("#6b7280"));

    const double startX = std::floor(wMinX / step) * step;
    for (double x = startX; x <= wMaxX; x += step) {
      if (std::abs(x) < step * 0.01)
        continue;
      const double sx = ox + x * m_zoom;
      p.drawLine(QPointF(sx, oy - 3), QPointF(sx, oy + 3));
      p.drawText(QRectF(sx - 28, oy + 4, 56, 14), Qt::AlignCenter, fmtVal(x));
    }

    const double startY = std::floor(wMinY / step) * step;
    for (double y = startY; y <= wMaxY; y += step) {
      if (std::abs(y) < step * 0.01)
        continue;
      const double sy = oy - y * m_zoom;
      p.drawLine(QPointF(ox - 3, sy), QPointF(ox + 3, sy));
      p.drawText(QRectF(ox + 6, sy - 7, 48, 14),
                 Qt::AlignLeft | Qt::AlignVCenter, fmtVal(y));
    }
    p.drawText(QRectF(ox + 4, oy + 4, 20, 14), Qt::AlignLeft, "0");
  }

  const Core::Rect w = m_res.win.normalized();
  const QPointF wTopLeft = toScreen(w.xmin, w.ymax);
  const QPointF wBotRight = toScreen(w.xmax, w.ymin);
  const QRectF winRect(wTopLeft, wBotRight);

  if (m_winLayer) {
    p.setPen(QPen(QColor(16, 185, 129, 55), 1.0, Qt::DashLine));
    p.drawLine(QPointF(wTopLeft.x(), 0), QPointF(wTopLeft.x(), height()));
    p.drawLine(QPointF(wBotRight.x(), 0), QPointF(wBotRight.x(), height()));
    p.drawLine(QPointF(0, wTopLeft.y()), QPointF(width(), wTopLeft.y()));
    p.drawLine(QPointF(0, wBotRight.y()), QPointF(width(), wBotRight.y()));

    p.setBrush(QColor(16, 185, 129, 22));
    p.setPen(QPen(QColor("#10b981"), 1.8));
    p.drawRect(winRect);

    if (m_labels) {
      p.setFont(QFont("monospace", 8));
      p.setPen(QColor("#34d399"));
      p.drawText(QPointF(wTopLeft.x() + 6, wBotRight.y() - 6),
                 QString("Window [%1..%2, %3..%4]")
                     .arg(fmtVal(w.xmin))
                     .arg(fmtVal(w.xmax))
                     .arg(fmtVal(w.ymin))
                     .arg(fmtVal(w.ymax)));
    }
  }
  if (m_origLayer) {
    if (m_res.mode == Core::Mode::LiangBarsky) {
      p.setPen(QPen(QColor(244, 63, 94, 185), 1.5, Qt::DashLine));
      for (const auto &s : m_res.segs) {
        const QPointF p1 = toScreen(s.orig.p1.x, s.orig.p1.y);
        const QPointF p2 = toScreen(s.orig.p2.x, s.orig.p2.y);
        p.drawLine(p1, p2);

        p.setBrush(QColor("#f43f5e"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(p1, 3.0, 3.0);
        p.drawEllipse(p2, 3.0, 3.0);

        if (m_labels) {
          p.setFont(QFont("monospace", 8));
          p.setPen(QColor("#fda4af"));
          const bool skipP1 = m_clipLayer && s.visible && s.tIn <= 1e-5;
          const bool skipP2 = m_clipLayer && s.visible && s.tOut >= 1.0 - 1e-5;
          if (!skipP1) {
            p.drawText(p1 + QPointF(6, -6), QString("P%1_1(%2,%3)")
                                                .arg(s.id)
                                                .arg(fmtVal(s.orig.p1.x))
                                                .arg(fmtVal(s.orig.p1.y)));
          }
          if (!skipP2) {
            p.drawText(p2 + QPointF(6, -6), QString("P%1_2(%2,%3)")
                                                .arg(s.id)
                                                .arg(fmtVal(s.orig.p2.x))
                                                .arg(fmtVal(s.orig.p2.y)));
          }
        }
        p.setPen(QPen(QColor(244, 63, 94, 185), 1.5, Qt::DashLine));
      }
    } else if (!m_res.origPoly.empty()) {
      QPolygonF poly;
      for (const auto &pt : m_res.origPoly) {
        poly << toScreen(pt.x, pt.y);
      }
      p.setBrush(QColor(244, 63, 94, 20));
      p.setPen(QPen(QColor(244, 63, 94, 190), 1.6, Qt::DashLine));
      p.drawPolygon(poly);

      for (size_t i = 0; i < m_res.origPoly.size(); ++i) {
        const QPointF sp = poly[static_cast<int>(i)];
        p.setBrush(QColor("#f43f5e"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(sp, 3.0, 3.0);

        if (m_labels) {
          bool overlapsClip = false;
          if (m_clipLayer) {
            for (const auto &cp : m_res.clipPoly) {
              if (std::hypot(cp.x - m_res.origPoly[i].x,
                             cp.y - m_res.origPoly[i].y) < 1e-5) {
                overlapsClip = true;
                break;
              }
            }
          }
          if (!overlapsClip) {
            p.setFont(QFont("monospace", 8));
            p.setPen(QColor("#fda4af"));
            p.drawText(sp + QPointF(6, -6),
                       QString("P%1(%2,%3)")
                           .arg(i + 1)
                           .arg(fmtVal(m_res.origPoly[i].x))
                           .arg(fmtVal(m_res.origPoly[i].y)));
          }
        }
      }
    }
  }

  if (m_clipLayer) {
    if (m_res.mode == Core::Mode::LiangBarsky) {
      for (const auto &s : m_res.segs) {
        if (!s.visible)
          continue;
        const QPointF c1 = toScreen(s.clip.p1.x, s.clip.p1.y);
        const QPointF c2 = toScreen(s.clip.p2.x, s.clip.p2.y);

        p.setPen(QPen(QColor("#3b82f6"), 3.0, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(c1, c2);

        p.setBrush(QColor("#f59e0b"));
        p.setPen(QPen(QColor("#fef08a"), 1.2));
        p.drawEllipse(c1, 4.2, 4.2);
        p.drawEllipse(c2, 4.2, 4.2);

        if (m_labels) {
          p.setFont(QFont("monospace", 8, QFont::Bold));
          p.setPen(QColor("#fef08a"));
          p.drawText(c1 + QPointF(7, 13), QString("Pвх(%1,%2) t=%3")
                                              .arg(fmtVal(s.clip.p1.x))
                                              .arg(fmtVal(s.clip.p1.y))
                                              .arg(fmtVal(s.tIn)));
          p.drawText(c2 + QPointF(7, -7), QString("Pвых(%1,%2) t=%3")
                                              .arg(fmtVal(s.clip.p2.x))
                                              .arg(fmtVal(s.clip.p2.y))
                                              .arg(fmtVal(s.tOut)));
        }
      }
    } else if (m_res.clipPoly.size() >= 3) {
      QPolygonF poly;
      for (const auto &pt : m_res.clipPoly) {
        poly << toScreen(pt.x, pt.y);
      }
      p.setBrush(QColor(59, 130, 246, 55));
      p.setPen(QPen(QColor("#3b82f6"), 2.8, Qt::SolidLine, Qt::RoundCap,
                    Qt::RoundJoin));
      p.drawPolygon(poly);

      for (size_t i = 0; i < m_res.clipPoly.size(); ++i) {
        const QPointF sp = poly[static_cast<int>(i)];
        p.setBrush(QColor("#f59e0b"));
        p.setPen(QPen(QColor("#fef08a"), 1.2));
        p.drawEllipse(sp, 4.0, 4.0);

        if (m_labels) {
          p.setFont(QFont("monospace", 8, QFont::Bold));
          p.setPen(QColor("#93c5fd"));
          p.drawText(sp + QPointF(7, 12),
                     QString("V%1(%2,%3)")
                         .arg(i + 1)
                         .arg(fmtVal(m_res.clipPoly[i].x))
                         .arg(fmtVal(m_res.clipPoly[i].y)));
        }
      }
    }
  }

  if (m_hasSel) {
    const QPointF sp = toScreen(m_selX, m_selY);
    p.setBrush(QColor(250, 204, 21, 90));
    p.setPen(QPen(QColor("#fef08a"), 2.0));
    p.drawEllipse(sp, 8.0, 8.0);
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
  const QPointF w = toWorld(e->position());
  emit hovered(w.x(), w.y());
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
    fit();
  }
}
