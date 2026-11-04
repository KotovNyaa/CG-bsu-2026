#pragma once

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

class Top : public QFrame {
  Q_OBJECT
public:
  explicit Top(QWidget *parent = nullptr);

  void setZoom(double z);

signals:
  void openTriggered();
  void saveTriggered();
  void zoomIn();
  void zoomOut();
  void fit();
  void zoom100();
  void reset();

  void gridToggled(bool v);
  void axesToggled(bool v);
  void winToggled(bool v);
  void origToggled(bool v);
  void clipToggled(bool v);
  void labelsToggled(bool v);

private:
  QLabel *m_zoomLabel{nullptr};
  QCheckBox *m_grid{nullptr};
  QCheckBox *m_axes{nullptr};
  QCheckBox *m_win{nullptr};
  QCheckBox *m_orig{nullptr};
  QCheckBox *m_clip{nullptr};
  QCheckBox *m_labels{nullptr};
};
