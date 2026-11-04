#include "Side.h"
#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHeaderView>
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
  buildLog(tab3);

  m_tabs->addTab(tab1, "Params");
  m_tabs->addTab(tab2, "Steps");
  m_tabs->addTab(tab3, "Log");

  root->addWidget(m_tabs);
}

void Side::buildParams(QWidget *w) {
  auto *lay = new QVBoxLayout(w);
  lay->setContentsMargins(8, 8, 8, 8);
  lay->setSpacing(8);

  auto *lblM = new QLabel("ALGORITHM (VARIANT 5)", w);
  lblM->setObjectName("sectionTitle");

  m_modeCombo = new Combo(w);
  m_modeCombo->addItem("Part 1: Liang-Barsky (Segments)",
                       static_cast<int>(Core::Mode::LiangBarsky));
  m_modeCombo->addItem("Part 2: Sutherland-Hodgman (Polygon)",
                       static_cast<int>(Core::Mode::SutherlandHodgman));

  connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int idx) {
            if (m_block)
              return;
            const auto m =
                static_cast<Core::Mode>(m_modeCombo->itemData(idx).toInt());
            updateModeLabels(m);
            emit modeChanged(m);
          });

  lay->addWidget(lblM);
  lay->addWidget(m_modeCombo);

  auto *lblW = new QLabel("CLIPPING WINDOW", w);
  lblW->setObjectName("sectionTitle");
  lay->addWidget(lblW);

  auto *winBox = new QWidget(w);
  auto *gWin = new QGridLayout(winBox);
  gWin->setContentsMargins(0, 0, 0, 0);
  gWin->setSpacing(6);

  auto makeSpin = [&](double val) {
    auto *sb = new QDoubleSpinBox(winBox);
    sb->setRange(-10000.0, 10000.0);
    sb->setDecimals(2);
    sb->setSingleStep(0.5);
    sb->setValue(val);
    return sb;
  };

  m_xmin = makeSpin(3.0);
  m_ymin = makeSpin(2.0);
  m_xmax = makeSpin(7.0);
  m_ymax = makeSpin(5.0);

  auto makeLbl = [&](const char *txt) {
    auto *l = new QLabel(txt, winBox);
    l->setObjectName("sliderLabel");
    return l;
  };

  gWin->addWidget(makeLbl("Xmin"), 0, 0);
  gWin->addWidget(m_xmin, 0, 1);
  gWin->addWidget(makeLbl("Ymin"), 0, 2);
  gWin->addWidget(m_ymin, 0, 3);
  gWin->addWidget(makeLbl("Xmax"), 1, 0);
  gWin->addWidget(m_xmax, 1, 1);
  gWin->addWidget(makeLbl("Ymax"), 1, 2);
  gWin->addWidget(m_ymax, 1, 3);

  auto connectWin = [&](QDoubleSpinBox *sb) {
    connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            [this]() { emitWindow(); });
  };
  connectWin(m_xmin);
  connectWin(m_ymin);
  connectWin(m_xmax);
  connectWin(m_ymax);

  lay->addWidget(winBox);

  m_geomTitle = new QLabel("SEGMENTS (X1 Y1 X2 Y2)", w);
  m_geomTitle->setObjectName("sectionTitle");
  lay->addWidget(m_geomTitle);

  m_geomEdit = new QPlainTextEdit(w);
  m_geomEdit->setMinimumHeight(140);
  connect(m_geomEdit, &QPlainTextEdit::textChanged, this, [this]() {
    if (m_block)
      return;
    emit geometryChanged(m_geomEdit->toPlainText());
  });
  lay->addWidget(m_geomEdit, 1);

  auto *lblPre = new QLabel("PRESETS", w);
  lblPre->setObjectName("sectionTitle");
  lay->addWidget(lblPre);

  auto *gPre = new QGridLayout();
  gPre->setSpacing(6);
  auto addPre = [&](const QString &txt, int row, int col, int id) {
    auto *btn = new QPushButton(txt, w);
    connect(btn, &QPushButton::clicked, this,
            [this, id]() { emit presetRequested(id); });
    gPre->addWidget(btn, row, col);
  };

  addPre("Lecture", 0, 0, 0);
  addPre("All Cases", 0, 1, 1);
  addPre("Star / Poly", 1, 0, 2);
  addPre("Clear", 1, 1, 3);

  lay->addLayout(gPre);

  auto *card = new QFrame(w);
  card->setObjectName("histCanvas");
  auto *cardLay = new QVBoxLayout(card);
  cardLay->setContentsMargins(8, 8, 8, 8);
  m_hintLabel = new QLabel(card);
  m_hintLabel->setObjectName("statsLabel");
  m_hintLabel->setWordWrap(true);
  cardLay->addWidget(m_hintLabel);
  lay->addWidget(card);

  updateModeLabels(Core::Mode::LiangBarsky);
}

void Side::buildSteps(QWidget *w) {
  auto *lay = new QVBoxLayout(w);
  lay->setContentsMargins(8, 8, 8, 8);
  lay->setSpacing(6);

  m_stepTable = new QTableWidget(w);
  m_stepTable->setColumnCount(4);
  m_stepTable->setHorizontalHeaderLabels(
      {"#", "Stage", "Parameters", "Action"});
  m_stepTable->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  m_stepTable->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  m_stepTable->horizontalHeader()->setSectionResizeMode(2,
                                                        QHeaderView::Stretch);
  m_stepTable->horizontalHeader()->setSectionResizeMode(3,
                                                        QHeaderView::Stretch);
  m_stepTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_stepTable->setSelectionMode(QAbstractItemView::SingleSelection);
  m_stepTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(m_stepTable, &QTableWidget::cellClicked, this,
          [this](int row, int) { emit stepSelected(row); });

  auto *btnCopy = new QPushButton("Copy Table to Clipboard", w);
  connect(btnCopy, &QPushButton::clicked, this, [this]() {
    QString text = "#\tStage\tParameters\tAction\n";
    for (int r = 0; r < m_stepTable->rowCount(); ++r) {
      for (int c = 0; c < 4; ++c) {
        auto *item = m_stepTable->item(r, c);
        text += (item ? item->text() : "") + (c == 3 ? "\n" : "\t");
      }
    }
    QApplication::clipboard()->setText(text);
  });

  lay->addWidget(m_stepTable);
  lay->addWidget(btnCopy);
}

void Side::buildLog(QWidget *w) {
  auto *lay = new QVBoxLayout(w);
  lay->setContentsMargins(8, 8, 8, 8);
  lay->setSpacing(6);

  m_logView = new QPlainTextEdit(w);
  m_logView->setReadOnly(true);

  auto *btnCopy = new QPushButton("Copy Protocol to Clipboard", w);
  connect(btnCopy, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(m_logView->toPlainText());
  });

  lay->addWidget(m_logView);
  lay->addWidget(btnCopy);
}

void Side::updateModeLabels(Core::Mode m) {
  if (m == Core::Mode::LiangBarsky) {
    m_geomTitle->setText("SEGMENTS (X1 Y1 X2 Y2)");
    m_hintLabel->setText(
        "Liang-Barsky Parametric Clipping:\n"
        "t * S_i <= Q_i (i=1..4: Bot, Left, Top, Right)\n"
        "File format: n, n lines [X1 Y1 X2 Y2], [Xmin Ymin Xmax Ymax].");
  } else {
    m_geomTitle->setText("POLYGON VERTICES (X Y)");
    m_hintLabel->setText(
        "Sutherland-Hodgman Polygon Clipping:\n"
        "Sequential clipping against Left, Right, Bottom, Top.\n"
        "Enter vertices [X Y] per line in traversal order.");
  }
}

void Side::setResult(const Core::Result &res) {
  m_stepTable->setRowCount(static_cast<int>(res.steps.size()));
  for (size_t i = 0; i < res.steps.size(); ++i) {
    const auto &s = res.steps[i];
    m_stepTable->setItem(static_cast<int>(i), 0,
                         new QTableWidgetItem(QString::number(s.i)));
    m_stepTable->setItem(static_cast<int>(i), 1,
                         new QTableWidgetItem(QString::fromStdString(s.stage)));
    m_stepTable->setItem(static_cast<int>(i), 2,
                         new QTableWidgetItem(QString::fromStdString(s.calc)));
    m_stepTable->setItem(static_cast<int>(i), 3,
                         new QTableWidgetItem(QString::fromStdString(s.act)));
  }
  m_logView->setPlainText(QString::fromStdString(res.log));
}

void Side::syncState(Core::Mode mode, const Core::Rect &win,
                     const QString &geomText) {
  m_block = true;
  for (int i = 0; i < m_modeCombo->count(); ++i) {
    if (m_modeCombo->itemData(i).toInt() == static_cast<int>(mode)) {
      m_modeCombo->setCurrentIndex(i);
      break;
    }
  }
  updateModeLabels(mode);
  m_xmin->setValue(win.xmin);
  m_ymin->setValue(win.ymin);
  m_xmax->setValue(win.xmax);
  m_ymax->setValue(win.ymax);
  m_geomEdit->setPlainText(geomText);
  m_block = false;
}

void Side::emitWindow() {
  if (m_block)
    return;
  emit windowChanged(m_xmin->value(), m_ymin->value(), m_xmax->value(),
                     m_ymax->value());
}
