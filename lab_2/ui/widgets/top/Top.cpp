#include "Top.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

Top::Top(QWidget *parent) : QFrame(parent) {
  setObjectName("top");
  setFixedHeight(40);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(10, 0, 10, 0);
  layout->setSpacing(6);

  auto makeBtn = [this](const QString &text) {
    auto *b = new QPushButton(text, this);
    b->setFocusPolicy(Qt::NoFocus);
    return b;
  };

  auto *openBtn = makeBtn("Open");
  auto *saveBtn = makeBtn("Save");
  auto *resetBtn = makeBtn("Reset");

  auto *sep1 = new QFrame(this);
  sep1->setFrameShape(QFrame::VLine);
  sep1->setStyleSheet("color: #23252b; background-color: #23252b; max-width: 1px;");

  m_origBtn = makeBtn("Original");
  m_origBtn->setCheckable(true);

  auto *sep2 = new QFrame(this);
  sep2->setFrameShape(QFrame::VLine);
  sep2->setStyleSheet("color: #23252b; background-color: #23252b; max-width: 1px;");

  auto *fitBtn = makeBtn("Fit");
  auto *zoom100Btn = makeBtn("1:1");
  auto *zoomOutBtn = makeBtn("-");
  auto *zoomInBtn = makeBtn("+");
  zoomOutBtn->setFixedWidth(28);
  zoomInBtn->setFixedWidth(28);

  m_zoomLabel = new QLabel("100%", this);
  m_zoomLabel->setObjectName("statusText");
  m_zoomLabel->setFixedWidth(46);
  m_zoomLabel->setAlignment(Qt::AlignCenter);

  layout->addWidget(openBtn);
  layout->addWidget(saveBtn);
  layout->addWidget(resetBtn);
  layout->addWidget(sep1);
  layout->addWidget(m_origBtn);
  layout->addWidget(sep2);
  layout->addWidget(fitBtn);
  layout->addWidget(zoom100Btn);
  layout->addWidget(zoomOutBtn);
  layout->addWidget(zoomInBtn);
  layout->addWidget(m_zoomLabel);
  layout->addStretch(1);

  connect(openBtn, &QPushButton::clicked, this, &Top::openTriggered);
  connect(saveBtn, &QPushButton::clicked, this, &Top::saveTriggered);
  connect(resetBtn, &QPushButton::clicked, this, &Top::resetTriggered);
  connect(m_origBtn, &QPushButton::toggled, this, &Top::showOriginalToggled);
  connect(fitBtn, &QPushButton::clicked, this, &Top::fitTriggered);
  connect(zoom100Btn, &QPushButton::clicked, this, &Top::zoom100Triggered);
  connect(zoomOutBtn, &QPushButton::clicked, this, &Top::zoomOutTriggered);
  connect(zoomInBtn, &QPushButton::clicked, this, &Top::zoomInTriggered);
}

void Top::setZoom(double z) {
  m_zoomLabel->setText(QString("%1%").arg(static_cast<int>(z * 100)));
}