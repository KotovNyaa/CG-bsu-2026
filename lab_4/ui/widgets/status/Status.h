#pragma once

#include "../../../core/Types.h"
#include <QFrame>
#include <QLabel>

class Status : public QFrame {
  Q_OBJECT
public:
  explicit Status(QWidget *parent = nullptr);

  void setCoord(double x, double y);
  void setStats(Core::Mode mode, int total, int vis, double us, bool convex);
  void setStatus(const QString &msg, int level = 0);

private:
  QLabel *m_coord{nullptr};
  QLabel *m_stats{nullptr};
  QLabel *m_mode{nullptr};
  QLabel *m_info{nullptr};
};
