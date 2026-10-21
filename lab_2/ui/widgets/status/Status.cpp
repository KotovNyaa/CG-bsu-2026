#include "Status.h"
#include <QHBoxLayout>
#include <QLabel>

Status::Status(QWidget *parent) : QFrame(parent) {
  setObjectName("status");
  setFixedHeight(26);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(12, 0, 12, 0);
  layout->setSpacing(16);

  m_resLabel = new QLabel("- × - px", this);
  m_resLabel->setObjectName("statusText");

  m_pixelLabel = new QLabel("X: -  Y: -  |  RGB: -", this);
  m_pixelLabel->setObjectName("statusText");

  m_statusLabel = new QLabel("Ready", this);
  m_statusLabel->setObjectName("statusBadge");

  layout->addWidget(m_resLabel);
  layout->addWidget(m_pixelLabel);
  layout->addStretch(1);
  layout->addWidget(m_statusLabel);
}

void Status::setResolution(int w, int h) {
  m_resLabel->setText(QString("%1 × %2 px  RGB888").arg(w).arg(h));
}

void Status::setPixelInfo(int x, int y, const QColor &c) {
  m_pixelLabel->setText(QString("X: %1  Y: %2  |  RGB(%3, %4, %5)")
                            .arg(x, 4)
                            .arg(y, 4)
                            .arg(c.red(), 3)
                            .arg(c.green(), 3)
                            .arg(c.blue(), 3));
}

void Status::setStatus(const QString &msg, int type) {
  m_statusLabel->setText(msg);
  if (type == 1)
    m_statusLabel->setStyleSheet("color: #10b981; font-weight: 700;");
  else if (type == 2)
    m_statusLabel->setStyleSheet("color: #ef4444; font-weight: 700;");
  else
    m_statusLabel->setStyleSheet("color: #8b949e; font-weight: 700;");
}