#include "Tools.h"
#include "Contrast.h"
#include "Filter.h"
#include "Hist.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

Tools::Tools(QWidget *parent) : QFrame(parent) {
  setObjectName("tools");
  setFixedWidth(290);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(10);

  auto makeSep = [this]() {
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #23252b; background-color: #23252b; max-height: 1px;");
    return sep;
  };

  m_hist = new Hist(this);
  m_contrast = new Contrast(this);
  m_filter = new Filter(this);

  auto *actionRow = new QHBoxLayout();
  actionRow->setSpacing(6);

  auto *commitBtn = new QPushButton("Commit", this);
  commitBtn->setObjectName("commitBtn");
  commitBtn->setFocusPolicy(Qt::NoFocus);
  commitBtn->setFixedHeight(28);

  auto *discardBtn = new QPushButton("Discard", this);
  discardBtn->setFocusPolicy(Qt::NoFocus);
  discardBtn->setFixedHeight(28);

  actionRow->addWidget(commitBtn, 1);
  actionRow->addWidget(discardBtn);

  layout->addWidget(m_hist);
  layout->addWidget(makeSep());
  layout->addWidget(m_contrast);
  layout->addWidget(makeSep());
  layout->addWidget(m_filter);
  layout->addWidget(makeSep());
  layout->addLayout(actionRow);
  layout->addStretch(1);

  connect(commitBtn, &QPushButton::clicked, this, &Tools::commitRequested);
  connect(discardBtn, &QPushButton::clicked, this, &Tools::discardRequested);
}