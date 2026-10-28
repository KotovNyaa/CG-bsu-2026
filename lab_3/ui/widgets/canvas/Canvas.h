#pragma once

#include <QFrame>
#include <QPointF>
#include "../../../core/Types.h"

class Canvas : public QFrame {
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);

    void setResult(const Core::Result &res);
    void setLine(int x1, int y1, int x2, int y2);
    void setCircle(int xc, int yc, int r);
    void setGrid(bool v);
    void setAxes(bool v);
    void setIdeal(bool v);
    void setDots(bool v);
    void setLabels(bool v);
    void setSel(int x, int y, bool has);
    void reset();
    void zoom100();
    void zoomIn();
    void zoomOut();

signals:
    void zoomChanged(double z);
    void hovered(int x, int y);

protected:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    void applyZoom(double nz, const QPointF &center);
    QPointF toScreen(double gx, double gy) const;
    QPoint fromScreen(const QPointF &sp) const;

    double m_zoom{24.0};
    QPointF m_offset{0.0, 0.0};
    QPoint m_lastPos;
    bool m_drag{false};
    bool m_init{false};

    Core::Result m_res;
    int m_x1{0}, m_y1{0}, m_x2{15}, m_y2{10};
    int m_xc{0}, m_yc{0}, m_r{12};
    bool m_grid{true};
    bool m_axes{true};
    bool m_ideal{true};
    bool m_dots{true};
    bool m_labels{false};
    bool m_hasSel{false};
    int m_selX{0}, m_selY{0};
    int m_hoverX{0}, m_hoverY{0};
    bool m_hasHover{false};
};