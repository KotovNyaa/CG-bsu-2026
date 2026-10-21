#pragma once

#include <QComboBox>

class Combo : public QComboBox {
  Q_OBJECT

public:
  explicit Combo(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *) override;
};