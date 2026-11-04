#include "Status.h"
#include <QHBoxLayout>

Status::Status(QWidget *parent) : QFrame(parent) {
  setObjectName("status");
  setFixedHeight(28);

  auto *lay = new QHBoxLayout(this);
  lay->setContentsMargins(12, 0, 12, 0);
  lay->setSpacing(16);

  m_coord = new QLabel("X: 0.00  Y: 0.00", this);
  m_coord->setObjectName("statusBadge");
  m_coord->setFixedWidth(155);

  m_stats = new QLabel("Segments: 0 (Visible: 0) | Time: 0.0 us", this);
  m_stats->setObjectName("statusText");
  m_stats->setFixedWidth(295);

  m_mode = new QLabel("Part 1: Liang-Barsky", this);
  m_mode->setObjectName("statusText");

  m_info = new QLabel("Ready", this);
  m_info->setObjectName("statusText");

  lay->addWidget(m_coord);
  lay->addWidget(m_stats);
  lay->addWidget(m_mode);
  lay->addStretch();
  lay->addWidget(m_info);
}

void Status::setCoord(double x, double y) {
  m_coord->setText(QString("X: %1  Y: %2").arg(x, 0, 'f', 2).arg(y, 0, 'f', 2));
}

void Status::setStats(Core::Mode mode, int total, int vis, double us,
                      bool convex) {
  if (mode == Core::Mode::LiangBarsky) {
    m_stats->setText(QString("Segments: %1 (Visible: %2) | Time: %3 us")
                         .arg(total)
                         .arg(vis)
                         .arg(us, 0, 'f', 1));
    m_mode->setText("Part 1: Liang-Barsky (Parametric)");
    m_mode->setStyleSheet("color: #8b949e;");
  } else {
    m_stats->setText(QString("In Verts: %1 (Out Verts: %2) | Time: %3 us")
                         .arg(total)
                         .arg(vis)
                         .arg(us, 0, 'f', 1));
    if (convex || total < 3) {
      m_mode->setText("Part 2: Sutherland-Hodgman (Convex)");
      m_mode->setStyleSheet("color: #8b949e;");
    } else {
      m_mode->setText("Part 2: Sutherland-Hodgman (Non-Convex!)");
      m_mode->setStyleSheet("color: #f59e0b;");
    }
  }
}

void Status::setStatus(const QString &msg, int level) {
  m_info->setText(msg);
  if (level == 1) {
    m_info->setStyleSheet("color: #10b981;");
  } else if (level == 2) {
    m_info->setStyleSheet("color: #f43f5e;");
  } else {
    m_info->setStyleSheet("color: #8b949e;");
  }
}
