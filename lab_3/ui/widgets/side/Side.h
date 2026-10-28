#pragma once

#include <QFrame>
#include <QSpinBox>
#include <QTableWidget>
#include <QTabWidget>
#include "../../../core/Types.h"
#include "../common/Combo.h"

class Side : public QFrame {
    Q_OBJECT
public:
    explicit Side(QWidget *parent = nullptr);

    void setResult(const Core::Result &res);
    void setReport(const Core::Report &rep);
    void setPrim(Core::Prim p);
    void setAlgo(Core::Algo a);

signals:
    void primChanged(Core::Prim p);
    void algoChanged(Core::Algo a);
    void lineChanged(int x1, int y1, int x2, int y2);
    void circleChanged(int xc, int yc, int r);
    void stepSelected(int idx);
    void benchRequested(int runs);

private:
    void buildParams(QWidget *w);
    void buildSteps(QWidget *w);
    void buildBench(QWidget *w);
    void updateAlgoList();
    void emitLine();
    void emitCircle();

    QTabWidget *m_tabs{nullptr};
    Combo *m_primCombo{nullptr};
    Combo *m_algoCombo{nullptr};

    QWidget *m_lineBox{nullptr};
    QSpinBox *m_x1{nullptr};
    QSpinBox *m_y1{nullptr};
    QSpinBox *m_x2{nullptr};
    QSpinBox *m_y2{nullptr};

    QWidget *m_circleBox{nullptr};
    QSpinBox *m_xc{nullptr};
    QSpinBox *m_yc{nullptr};
    QSpinBox *m_r{nullptr};

    QTableWidget *m_stepTable{nullptr};
    Combo *m_benchRuns{nullptr};
    QTableWidget *m_benchTable{nullptr};

    bool m_block{false};
};