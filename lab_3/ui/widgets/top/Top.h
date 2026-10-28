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
    void zoomIn();
    void zoomOut();
    void zoom100();
    void reset();
    void gridToggled(bool v);
    void axesToggled(bool v);
    void idealToggled(bool v);
    void dotsToggled(bool v);
    void labelsToggled(bool v);

private:
    QLabel *m_zoomLabel{nullptr};
    QCheckBox *m_grid{nullptr};
    QCheckBox *m_axes{nullptr};
    QCheckBox *m_ideal{nullptr};
    QCheckBox *m_dots{nullptr};
    QCheckBox *m_labels{nullptr};
};