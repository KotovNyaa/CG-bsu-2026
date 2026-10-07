#include "PreviewBox.h"
#include <QPainter>

PreviewBox::PreviewBox(QWidget *parent) : QWidget(parent) {
  setObjectName("previewBox");
  setFixedSize(90, 72);
}

void PreviewBox::setColor(const QColor &color) {
  m_color = color;
  update();
}

void PreviewBox::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);
  p.setBrush(m_color);
  p.drawRoundedRect(rect(), 4, 4);
}