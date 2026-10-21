#include "Combo.h"
#include <QListView>
#include <QPainter>
#include <QStyleOptionComboBox>
#include <QStylePainter>

Combo::Combo(QWidget *parent) : QComboBox(parent) {
  setFocusPolicy(Qt::NoFocus);
  auto *v = new QListView(this);
  v->setObjectName("comboView");
  setView(v);
}

void Combo::paintEvent(QPaintEvent *) {
  QStylePainter sp(this);
  QStyleOptionComboBox opt;
  initStyleOption(&opt);
  opt.subControls &= ~QStyle::SC_ComboBoxArrow;
  sp.drawComplexControl(QStyle::CC_ComboBox, opt);
  sp.drawControl(QStyle::CE_ComboBoxLabel, opt);

  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::NoPen);
  p.setBrush(QColor("#8b949e"));

  const double x = width() - 12.0;
  const double y = height() * 0.5 - 1.0;
  const QPointF pts[3] = {{x - 3.5, y}, {x + 3.5, y}, {x, y + 4.0}};
  p.drawPolygon(pts, 3);
}