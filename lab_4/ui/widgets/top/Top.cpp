#include "Top.h"
#include <QHBoxLayout>

Top::Top(QWidget *parent) : QFrame(parent) {
  setObjectName("top");
  setFixedHeight(42);

  auto *lay = new QHBoxLayout(this);
  lay->setContentsMargins(12, 0, 12, 0);
  lay->setSpacing(8);

  auto *btnOpen = new QPushButton("Open", this);
  connect(btnOpen, &QPushButton::clicked, this, &Top::openTriggered);

  auto *btnSave = new QPushButton("Save", this);
  connect(btnSave, &QPushButton::clicked, this, &Top::saveTriggered);

  auto *btnIn = new QPushButton("+", this);
  btnIn->setFixedWidth(28);
  connect(btnIn, &QPushButton::clicked, this, &Top::zoomIn);

  auto *btnOut = new QPushButton("-", this);
  btnOut->setFixedWidth(28);
  connect(btnOut, &QPushButton::clicked, this, &Top::zoomOut);

  auto *btnFit = new QPushButton("Fit", this);
  connect(btnFit, &QPushButton::clicked, this, &Top::fit);

  auto *btn100 = new QPushButton("100%", this);
  connect(btn100, &QPushButton::clicked, this, &Top::zoom100);

  auto *btnReset = new QPushButton("Reset", this);
  connect(btnReset, &QPushButton::clicked, this, &Top::reset);

  m_zoomLabel = new QLabel("48 px", this);
  m_zoomLabel->setObjectName("zoomText");
  m_zoomLabel->setFixedWidth(56);

  m_grid = new QCheckBox("Grid", this);
  m_grid->setChecked(true);
  connect(m_grid, &QCheckBox::toggled, this, &Top::gridToggled);

  m_axes = new QCheckBox("Axes", this);
  m_axes->setChecked(true);
  connect(m_axes, &QCheckBox::toggled, this, &Top::axesToggled);

  m_win = new QCheckBox("Window", this);
  m_win->setChecked(true);
  connect(m_win, &QCheckBox::toggled, this, &Top::winToggled);

  m_orig = new QCheckBox("Original", this);
  m_orig->setChecked(true);
  connect(m_orig, &QCheckBox::toggled, this, &Top::origToggled);

  m_clip = new QCheckBox("Clipped", this);
  m_clip->setChecked(true);
  connect(m_clip, &QCheckBox::toggled, this, &Top::clipToggled);

  m_labels = new QCheckBox("Labels", this);
  m_labels->setChecked(false);
  connect(m_labels, &QCheckBox::toggled, this, &Top::labelsToggled);

  lay->addWidget(btnOpen);
  lay->addWidget(btnSave);
  lay->addSpacing(8);
  lay->addWidget(btnIn);
  lay->addWidget(btnOut);
  lay->addWidget(btnFit);
  lay->addWidget(btn100);
  lay->addWidget(btnReset);
  lay->addWidget(m_zoomLabel);
  lay->addSpacing(12);
  lay->addWidget(m_grid);
  lay->addWidget(m_axes);
  lay->addWidget(m_win);
  lay->addWidget(m_orig);
  lay->addWidget(m_clip);
  lay->addWidget(m_labels);
  lay->addStretch();
}

void Top::setZoom(double z) {
  if (z >= 10.0) {
    m_zoomLabel->setText(QString::number(static_cast<int>(z)) + " px");
  } else {
    m_zoomLabel->setText(QString::number(z, 'f', 1) + " px");
  }
}
