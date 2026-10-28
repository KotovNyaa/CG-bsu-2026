#include "Status.h"
#include <QHBoxLayout>

Status::Status(QWidget *parent) : QFrame(parent) {
    setObjectName("status");
    setFixedHeight(28);

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(12, 0, 12, 0);
    lay->setSpacing(16);

    m_coord = new QLabel("X: 0  Y: 0", this);
    m_coord->setObjectName("statusBadge");

    m_stats = new QLabel("Pixels: 0 | Time: 0 us", this);
    m_stats->setObjectName("statusText");

    m_bind = new QLabel("Binding: Cell [X, X+1) x [Y, Y+1)", this);
    m_bind->setObjectName("statusText");

    m_info = new QLabel("Ready", this);
    m_info->setObjectName("statusText");

    lay->addWidget(m_coord);
    lay->addWidget(m_stats);
    lay->addWidget(m_bind);
    lay->addStretch();
    lay->addWidget(m_info);
}

void Status::setCoord(int x, int y) {
    m_coord->setText(QString("X: %1  Y: %2").arg(x).arg(y));
}

void Status::setStats(int count, double us) {
    m_stats->setText(QString("Pixels: %1 | Time: %2 us").arg(count).arg(us, 0, 'f', 1));
}

void Status::setInfo(const QString &msg) {
    m_info->setText(msg);
}