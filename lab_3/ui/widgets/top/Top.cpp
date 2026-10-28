#include "Top.h"
#include <QHBoxLayout>

Top::Top(QWidget *parent) : QFrame(parent) {
    setObjectName("top");
    setFixedHeight(42);

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(12, 0, 12, 0);
    lay->setSpacing(8);

    auto *btnIn = new QPushButton("+", this);
    btnIn->setFixedWidth(28);
    connect(btnIn, &QPushButton::clicked, this, &Top::zoomIn);

    auto *btnOut = new QPushButton("-", this);
    btnOut->setFixedWidth(28);
    connect(btnOut, &QPushButton::clicked, this, &Top::zoomOut);

    auto *btn100 = new QPushButton("100%", this);
    connect(btn100, &QPushButton::clicked, this, &Top::zoom100);

    auto *btnReset = new QPushButton("Reset", this);
    connect(btnReset, &QPushButton::clicked, this, &Top::reset);

    m_zoomLabel = new QLabel("24 px", this);
    m_zoomLabel->setObjectName("zoomText");
    m_zoomLabel->setFixedWidth(56);

    m_grid = new QCheckBox("Grid", this);
    m_grid->setChecked(true);
    connect(m_grid, &QCheckBox::toggled, this, &Top::gridToggled);

    m_axes = new QCheckBox("Axes", this);
    m_axes->setChecked(true);
    connect(m_axes, &QCheckBox::toggled, this, &Top::axesToggled);

    m_ideal = new QCheckBox("Ideal", this);
    m_ideal->setChecked(true);
    connect(m_ideal, &QCheckBox::toggled, this, &Top::idealToggled);

    m_dots = new QCheckBox("Centers", this);
    m_dots->setChecked(true);
    connect(m_dots, &QCheckBox::toggled, this, &Top::dotsToggled);

    m_labels = new QCheckBox("Labels", this);
    m_labels->setChecked(false);
    connect(m_labels, &QCheckBox::toggled, this, &Top::labelsToggled);

    lay->addWidget(btnIn);
    lay->addWidget(btnOut);
    lay->addWidget(btn100);
    lay->addWidget(btnReset);
    lay->addWidget(m_zoomLabel);
    lay->addSpacing(16);
    lay->addWidget(m_grid);
    lay->addWidget(m_axes);
    lay->addWidget(m_ideal);
    lay->addWidget(m_dots);
    lay->addWidget(m_labels);
    lay->addStretch();
}

void Top::setZoom(double z) {
    m_zoomLabel->setText(QString::number(static_cast<int>(z)) + " px");
}