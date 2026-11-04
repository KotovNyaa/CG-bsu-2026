#pragma once

#include "../../../core/Types.h"
#include <QFrame>
#include <QPointF>

class Canvas : public QFrame {
  Q_OBJECT
public:
  explicit Canvas(QWidget *parent = nullptr);

  void setResult(const Core::Result &res);
  void setGrid(bool v);
  void setAxes(bool v);
  void setWinLayer(bool v);
  void setOrigLayer(bool v);
  void setClipLayer(bool v);
  void setLabels(bool v);
  void setSel(double x, double y, bool has);

  void fit();
  void reset();
  void zoom100();
  void zoomIn();
  void zoomOut();

signals:
  void zoomChanged(double z);
  void hovered(double x, double y);

protected:
  void paintEvent(QPaintEvent *e) override;
  void wheelEvent(QWheelEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void resizeEvent(QResizeEvent *e) override;

private:
  void applyZoom(double nz, const QPointF &center);
  QPointF toScreen(double wx, double wy) const;
  QPointF toWorld(const QPointF &sp) const;
  double calcStep() const;
  static QString fmtVal(double v);

  Core::Result m_res;
  QPointF m_offset{0.0, 0.0};
  double m_zoom{48.0};

  bool m_grid{true};
  bool m_axes{true};
  bool m_winLayer{true};
  bool m_origLayer{true};
  bool m_clipLayer{true};
  bool m_labels{false};

  bool m_hasSel{false};
  double m_selX{0.0};
  double m_selY{0.0};

  bool m_drag{false};
  QPoint m_lastPos;
  bool m_init{false};
};
