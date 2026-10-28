#pragma once

#include <QObject>
#include "../core/Types.h"

class Controller : public QObject {
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

    void setPrim(Core::Prim p);
    void setAlgo(Core::Algo a);
    void setLine(int x1, int y1, int x2, int y2);
    void setCircle(int xc, int yc, int r);
    void setGrid(bool v);
    void setAxes(bool v);
    void setIdeal(bool v);
    void setDots(bool v);
    void setLabels(bool v);
    void selectStep(int idx);
    void clearSel();
    void runBench(int runs);
    void recalc();

    Core::Prim prim() const { return m_prim; }
    Core::Algo algo() const { return m_algo; }
    int x1() const { return m_x1; }
    int y1() const { return m_y1; }
    int x2() const { return m_x2; }
    int y2() const { return m_y2; }
    int xc() const { return m_xc; }
    int yc() const { return m_yc; }
    int r() const { return m_r; }
    bool grid() const { return m_grid; }
    bool axes() const { return m_axes; }
    bool ideal() const { return m_ideal; }
    bool dots() const { return m_dots; }
    bool labels() const { return m_labels; }
    bool hasSel() const { return m_hasSel; }
    int selX() const { return m_selX; }
    int selY() const { return m_selY; }
    const Core::Result &result() const { return m_res; }
    const Core::Report &report() const { return m_rep; }

signals:
    void resultChanged(const Core::Result &res);
    void reportChanged(const Core::Report &rep);
    void viewChanged();
    void selChanged(int x, int y, bool hasSel);

private:
    Core::Prim m_prim{Core::Prim::Line};
    Core::Algo m_algo{Core::Algo::Bresenham};
    int m_x1{0};
    int m_y1{0};
    int m_x2{15};
    int m_y2{10};
    int m_xc{0};
    int m_yc{0};
    int m_r{12};
    bool m_grid{true};
    bool m_axes{true};
    bool m_ideal{true};
    bool m_dots{true};
    bool m_labels{false};
    bool m_hasSel{false};
    int m_selX{0};
    int m_selY{0};
    Core::Result m_res;
    Core::Report m_rep;
};