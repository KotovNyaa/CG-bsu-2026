#pragma once

#include "../../../core/Types.h"
#include "../common/Combo.h"
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QTableWidget>

class Side : public QFrame {
  Q_OBJECT
public:
  explicit Side(QWidget *parent = nullptr);

  void setResult(const Core::Result &res);
  void syncState(Core::Mode mode, const Core::Rect &win,
                 const QString &geomText);

signals:
  void modeChanged(Core::Mode m);
  void windowChanged(double xmin, double ymin, double xmax, double ymax);
  void geometryChanged(const QString &text);
  void presetRequested(int index);
  void stepSelected(int index);

private:
  void buildParams(QWidget *w);
  void buildSteps(QWidget *w);
  void buildLog(QWidget *w);
  void updateModeLabels(Core::Mode m);
  void emitWindow();

  QTabWidget *m_tabs{nullptr};
  Combo *m_modeCombo{nullptr};

  QDoubleSpinBox *m_xmin{nullptr};
  QDoubleSpinBox *m_ymin{nullptr};
  QDoubleSpinBox *m_xmax{nullptr};
  QDoubleSpinBox *m_ymax{nullptr};

  QLabel *m_geomTitle{nullptr};
  QPlainTextEdit *m_geomEdit{nullptr};
  QLabel *m_hintLabel{nullptr};

  QTableWidget *m_stepTable{nullptr};
  QPlainTextEdit *m_logView{nullptr};

  bool m_block{false};
};
