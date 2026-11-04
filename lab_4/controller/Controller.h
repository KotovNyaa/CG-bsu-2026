#pragma once

#include "../core/Types.h"
#include <QObject>
#include <QString>

class Controller : public QObject {
  Q_OBJECT
public:
  explicit Controller(QObject *parent = nullptr);

  void setMode(Core::Mode m);
  void setWindow(double xmin, double ymin, double xmax, double ymax);
  void setGeometryText(const QString &text);
  void applyPreset(int index);

  void load(const QString &path);
  void save(const QString &path);

  void setGrid(bool v);
  void setAxes(bool v);
  void setWinLayer(bool v);
  void setOrigLayer(bool v);
  void setClipLayer(bool v);
  void setLabels(bool v);

  void selectStep(int idx);
  void clearSel();
  void recalc();

  Core::Mode mode() const { return m_mode; }
  const Core::Rect &window() const { return m_win; }
  const std::vector<Core::Segment> &segments() const { return m_segs; }
  const std::vector<Core::Point> &polygon() const { return m_poly; }
  QString geometryText() const;

  bool grid() const { return m_grid; }
  bool axes() const { return m_axes; }
  bool winLayer() const { return m_winLayer; }
  bool origLayer() const { return m_origLayer; }
  bool clipLayer() const { return m_clipLayer; }
  bool labels() const { return m_labels; }

  bool hasSel() const { return m_hasSel; }
  double selX() const { return m_selX; }
  double selY() const { return m_selY; }

  const Core::Result &result() const { return m_res; }

signals:
  void resultChanged(const Core::Result &res);
  void stateSynced();
  void viewChanged();
  void fitRequested();
  void selChanged(double x, double y, bool hasSel);
  void statusChanged(const QString &msg, int level);

private:
  Core::Mode m_mode{Core::Mode::LiangBarsky};
  Core::Rect m_win{3.0, 2.0, 7.0, 5.0};
  std::vector<Core::Segment> m_segs;
  std::vector<Core::Point> m_poly;

  bool m_grid{true};
  bool m_axes{true};
  bool m_winLayer{true};
  bool m_origLayer{true};
  bool m_clipLayer{true};
  bool m_labels{false};

  bool m_hasSel{false};
  double m_selX{0.0};
  double m_selY{0.0};

  Core::Result m_res;
};
