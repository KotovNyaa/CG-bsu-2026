#pragma once

#include <QFrame>
#include <QLabel>

class Status : public QFrame {
    Q_OBJECT
public:
    explicit Status(QWidget *parent = nullptr);
    void setCoord(int x, int y);
    void setStats(int count, double us);
    void setInfo(const QString &msg);

private:
    QLabel *m_coord{nullptr};
    QLabel *m_stats{nullptr};
    QLabel *m_bind{nullptr};
    QLabel *m_info{nullptr};
};