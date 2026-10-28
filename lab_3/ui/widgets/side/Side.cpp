#include "Side.h"
#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

Side::Side(QWidget *parent) : QFrame(parent) {
    setObjectName("side");
    setFixedWidth(360);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    m_tabs = new QTabWidget(this);
    auto *tab1 = new QWidget(this);
    auto *tab2 = new QWidget(this);
    auto *tab3 = new QWidget(this);

    buildParams(tab1);
    buildSteps(tab2);
    buildBench(tab3);

    m_tabs->addTab(tab1, "Params");
    m_tabs->addTab(tab2, "Steps");
    m_tabs->addTab(tab3, "Bench");

    root->addWidget(m_tabs);
    updateAlgoList();
}

void Side::buildParams(QWidget *w) {
    auto *lay = new QVBoxLayout(w);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(8);

    auto *lblP = new QLabel("PRIMITIVE", w);
    lblP->setObjectName("sectionTitle");
    m_primCombo = new Combo(w);
    m_primCombo->addItem("Line", static_cast<int>(Core::Prim::Line));
    m_primCombo->addItem("Circle", static_cast<int>(Core::Prim::Circle));

    auto *lblA = new QLabel("ALGORITHM", w);
    lblA->setObjectName("sectionTitle");
    m_algoCombo = new Combo(w);

    connect(m_primCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (m_block) return;
        const auto p = static_cast<Core::Prim>(m_primCombo->itemData(idx).toInt());
        updateAlgoList();
        emit primChanged(p);
    });

    connect(m_algoCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (m_block) return;
        emit algoChanged(static_cast<Core::Algo>(m_algoCombo->itemData(idx).toInt()));
    });

    lay->addWidget(lblP);
    lay->addWidget(m_primCombo);
    lay->addWidget(lblA);
    lay->addWidget(m_algoCombo);

    m_lineBox = new QWidget(w);
    auto *gLine = new QGridLayout(m_lineBox);
    gLine->setContentsMargins(0, 0, 0, 0);

    m_x1 = new QSpinBox(m_lineBox); m_x1->setRange(-500, 500); m_x1->setValue(0);
    m_y1 = new QSpinBox(m_lineBox); m_y1->setRange(-500, 500); m_y1->setValue(0);
    m_x2 = new QSpinBox(m_lineBox); m_x2->setRange(-500, 500); m_x2->setValue(15);
    m_y2 = new QSpinBox(m_lineBox); m_y2->setRange(-500, 500); m_y2->setValue(10);

    auto makeLbl = [&](const char *txt) {
        auto *l = new QLabel(txt, w);
        l->setObjectName("sliderLabel");
        return l;
    };

    gLine->addWidget(makeLbl("X1"), 0, 0); gLine->addWidget(m_x1, 0, 1);
    gLine->addWidget(makeLbl("Y1"), 0, 2); gLine->addWidget(m_y1, 0, 3);
    gLine->addWidget(makeLbl("X2"), 1, 0); gLine->addWidget(m_x2, 1, 1);
    gLine->addWidget(makeLbl("Y2"), 1, 2); gLine->addWidget(m_y2, 1, 3);

    auto connectLine = [&](QSpinBox *sb) {
        connect(sb, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() { emitLine(); });
    };
    connectLine(m_x1); connectLine(m_y1); connectLine(m_x2); connectLine(m_y2);

    m_circleBox = new QWidget(w);
    auto *gCirc = new QGridLayout(m_circleBox);
    gCirc->setContentsMargins(0, 0, 0, 0);

    m_xc = new QSpinBox(m_circleBox); m_xc->setRange(-500, 500); m_xc->setValue(0);
    m_yc = new QSpinBox(m_circleBox); m_yc->setRange(-500, 500); m_yc->setValue(0);
    m_r = new QSpinBox(m_circleBox); m_r->setRange(1, 500); m_r->setValue(12);

    gCirc->addWidget(makeLbl("Xc"), 0, 0); gCirc->addWidget(m_xc, 0, 1);
    gCirc->addWidget(makeLbl("Yc"), 0, 2); gCirc->addWidget(m_yc, 0, 3);
    gCirc->addWidget(makeLbl("R"), 1, 0); gCirc->addWidget(m_r, 1, 1);

    auto connectCirc = [&](QSpinBox *sb) {
        connect(sb, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() { emitCircle(); });
    };
    connectCirc(m_xc); connectCirc(m_yc); connectCirc(m_r);

    m_circleBox->setVisible(false);

    lay->addWidget(m_lineBox);
    lay->addWidget(m_circleBox);

    auto *lblPre = new QLabel("PRESETS", w);
    lblPre->setObjectName("sectionTitle");
    lay->addWidget(lblPre);

    auto *gPre = new QGridLayout();
    auto addPre = [&](const QString &txt, int row, int col, auto fn) {
        auto *btn = new QPushButton(txt, w);
        connect(btn, &QPushButton::clicked, this, fn);
        gPre->addWidget(btn, row, col);
    };

    addPre("Shallow", 0, 0, [this]() {
        m_primCombo->setCurrentIndex(0);
        m_x1->setValue(0); m_y1->setValue(0); m_x2->setValue(16); m_y2->setValue(5);
    });
    addPre("Steep", 0, 1, [this]() {
        m_primCombo->setCurrentIndex(0);
        m_x1->setValue(0); m_y1->setValue(0); m_x2->setValue(5); m_y2->setValue(16);
    });
    addPre("Diagonal", 1, 0, [this]() {
        m_primCombo->setCurrentIndex(0);
        m_x1->setValue(0); m_y1->setValue(0); m_x2->setValue(12); m_y2->setValue(12);
    });
    addPre("Circle R=12", 1, 1, [this]() {
        m_primCombo->setCurrentIndex(1);
        m_xc->setValue(0); m_yc->setValue(0); m_r->setValue(12);
    });

    lay->addLayout(gPre);

    auto *card = new QFrame(w);
    card->setObjectName("histCanvas");
    auto *cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(8, 8, 8, 8);
    auto *cardTxt = new QLabel("Grid Mapping:\nInteger point (X, Y) defines raster cell [X, X+1) x [Y, Y+1).\nCenter is at (X+0.5, Y+0.5).", card);
    cardTxt->setObjectName("statsLabel");
    cardLay->addWidget(cardTxt);
    lay->addWidget(card);

    lay->addStretch();
}

void Side::buildSteps(QWidget *w) {
    auto *lay = new QVBoxLayout(w);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(6);

    m_stepTable = new QTableWidget(w);
    m_stepTable->setColumnCount(5);
    m_stepTable->setHorizontalHeaderLabels({"#", "X", "Y", "Err", "Act"});
    m_stepTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_stepTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_stepTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_stepTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_stepTable, &QTableWidget::cellClicked, this, [this](int row, int) {
        emit stepSelected(row);
    });

    auto *btnCopy = new QPushButton("Copy Table to Clipboard", w);
    connect(btnCopy, &QPushButton::clicked, this, [this]() {
        QString text = "Step\tX\tY\tError\tAction\n";
        for (int r = 0; r < m_stepTable->rowCount(); ++r) {
            for (int c = 0; c < 5; ++c) {
                auto *item = m_stepTable->item(r, c);
                text += (item ? item->text() : "") + (c == 4 ? "\n" : "\t");
            }
        }
        QApplication::clipboard()->setText(text);
    });

    lay->addWidget(m_stepTable);
    lay->addWidget(btnCopy);
}

void Side::buildBench(QWidget *w) {
    auto *lay = new QVBoxLayout(w);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(8);

    auto *lblRuns = new QLabel("ITERATIONS", w);
    lblRuns->setObjectName("sectionTitle");
    m_benchRuns = new Combo(w);
    m_benchRuns->addItem("10 000", 10000);
    m_benchRuns->addItem("50 000", 50000);
    m_benchRuns->addItem("100 000", 100000);
    m_benchRuns->addItem("500 000", 500000);
    m_benchRuns->setCurrentIndex(2);

    auto *btnRun = new QPushButton("Run Benchmark", w);
    btnRun->setObjectName("commitBtn");
    connect(btnRun, &QPushButton::clicked, this, [this]() {
        emit benchRequested(m_benchRuns->currentData().toInt());
    });

    m_benchTable = new QTableWidget(w);
    m_benchTable->setColumnCount(4);
    m_benchTable->setHorizontalHeaderLabels({"Algorithm", "Total ms", "us/call", "Speedup"});
    m_benchTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_benchTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *note = new QLabel("Bresenham uses only integer addition and bit shifts, avoiding floating-point rounding and division, making it significantly faster than Step and DDA.", w);
    note->setObjectName("statsLabel");
    note->setWordWrap(true);

    lay->addWidget(lblRuns);
    lay->addWidget(m_benchRuns);
    lay->addWidget(btnRun);
    lay->addWidget(m_benchTable);
    lay->addWidget(note);
}

void Side::updateAlgoList() {
    m_block = true;
    m_algoCombo->clear();
    const auto p = static_cast<Core::Prim>(m_primCombo->currentData().toInt());
    if (p == Core::Prim::Line) {
        m_lineBox->setVisible(true);
        m_circleBox->setVisible(false);
        m_algoCombo->addItem("Step-by-Step", static_cast<int>(Core::Algo::Step));
        m_algoCombo->addItem("DDA", static_cast<int>(Core::Algo::Dda));
        m_algoCombo->addItem("Bresenham", static_cast<int>(Core::Algo::Bresenham));
        m_algoCombo->addItem("Castle-Pitteway", static_cast<int>(Core::Algo::Castle));
        m_algoCombo->addItem("Wu (Antialiased)", static_cast<int>(Core::Algo::Wu));
        m_algoCombo->setCurrentIndex(2);
    } else {
        m_lineBox->setVisible(false);
        m_circleBox->setVisible(true);
        m_algoCombo->addItem("Bresenham Circle", static_cast<int>(Core::Algo::Circle));
        m_algoCombo->setCurrentIndex(0);
    }
    m_block = false;
}

void Side::setResult(const Core::Result &res) {
    m_stepTable->setRowCount(static_cast<int>(res.steps.size()));
    for (size_t i = 0; i < res.steps.size(); ++i) {
        const auto &s = res.steps[i];
        m_stepTable->setItem(i, 0, new QTableWidgetItem(QString::number(s.i)));
        m_stepTable->setItem(i, 1, new QTableWidgetItem(QString::number(s.x)));
        m_stepTable->setItem(i, 2, new QTableWidgetItem(QString::number(s.y)));
        m_stepTable->setItem(i, 3, new QTableWidgetItem(QString::number(s.err, 'f', 2)));
        m_stepTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(s.act)));
    }
}

void Side::setReport(const Core::Report &rep) {
    m_benchTable->setRowCount(static_cast<int>(rep.items.size()));
    for (size_t i = 0; i < rep.items.size(); ++i) {
        const auto &it = rep.items[i];
        m_benchTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(it.name)));
        m_benchTable->setItem(i, 1, new QTableWidgetItem(QString::number(it.total, 'f', 2)));
        m_benchTable->setItem(i, 2, new QTableWidgetItem(QString::number(it.avg, 'f', 3)));
        m_benchTable->setItem(i, 3, new QTableWidgetItem(QString("%1x").arg(it.boost, 0, 'f', 2)));
    }
}

void Side::setPrim(Core::Prim p) {
    for (int i = 0; i < m_primCombo->count(); ++i) {
        if (m_primCombo->itemData(i).toInt() == static_cast<int>(p)) {
            m_primCombo->setCurrentIndex(i);
            break;
        }
    }
}

void Side::setAlgo(Core::Algo a) {
    for (int i = 0; i < m_algoCombo->count(); ++i) {
        if (m_algoCombo->itemData(i).toInt() == static_cast<int>(a)) {
            m_algoCombo->setCurrentIndex(i);
            break;
        }
    }
}

void Side::emitLine() {
    if (m_block) return;
    emit lineChanged(m_x1->value(), m_y1->value(), m_x2->value(), m_y2->value());
}

void Side::emitCircle() {
    if (m_block) return;
    emit circleChanged(m_xc->value(), m_yc->value(), m_r->value());
}