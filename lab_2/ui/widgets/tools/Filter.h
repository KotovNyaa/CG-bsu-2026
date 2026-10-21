#pragma once

#include <QWidget>

class Combo;

class Filter : public QWidget {
  Q_OBJECT

public:
  explicit Filter(QWidget *parent = nullptr);
  void resetState();

signals:
  void filterChanged(int type, int radius);

private:
  void emitChange();

  Combo *m_typeCombo{nullptr};
  Combo *m_sizeCombo{nullptr};
};