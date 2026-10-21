#pragma once

#include <QFrame>
#include <QImage>

class Viewer : public QFrame {
  Q_OBJECT

public:
  explicit Viewer(QWidget *parent = nullptr);
  void setImage(const QImage &img);
  void fit();
  void setZoom100();
  void zoomIn();
  void zoomOut();

signals:
  void zoomChanged(double zoom);
  void pixelHovered(int x, int y, const QColor &color);

protected:
  void paintEvent(QPaintEvent *) override;
  void wheelEvent(QWheelEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void showEvent(QShowEvent *e) override;
  void resizeEvent(QResizeEvent *) override;

private:
  void applyZoom(double newZoom, const QPointF &center);

  QImage m_img;
  double m_zoom{1.0};
  QPointF m_offset{0.0, 0.0};
  QPoint m_lastMousePos;
  bool m_dragging{false};
  bool m_initialized{false};
};