#include "Controller.h"
#include "../core/Io.h"
#include "../core/Ops.h"

Controller::Controller(QObject *parent) : QObject(parent) {
  m_win = {3.0, 2.0, 7.0, 5.0};
  m_segs = {{{2.0, 1.0}, {8.0, 6.0}}};
  m_poly = {{1.5, 3.5}, {4.5, 6.2}, {8.2, 4.5}, {6.5, 1.0}, {2.5, 1.2}};
  recalc();
}

void Controller::setMode(Core::Mode m) {
  if (m_mode == m)
    return;
  m_mode = m;
  m_hasSel = false;
  emit stateSynced();
  recalc();
  emit fitRequested();
}

void Controller::setWindow(double xmin, double ymin, double xmax, double ymax) {
  m_win = {xmin, ymin, xmax, ymax};
  m_hasSel = false;
  recalc();
}

void Controller::setGeometryText(const QString &text) {
  const std::string raw = text.toStdString();
  if (m_mode == Core::Mode::LiangBarsky) {
    m_segs = Core::Io::parseSegments(raw);
  } else {
    m_poly = Core::Io::parsePolygon(raw);
  }
  m_hasSel = false;
  recalc();
}

QString Controller::geometryText() const {
  if (m_mode == Core::Mode::LiangBarsky) {
    return QString::fromStdString(Core::Io::formatSegments(m_segs));
  }
  return QString::fromStdString(Core::Io::formatPolygon(m_poly));
}

void Controller::applyPreset(int index) {
  m_hasSel = false;

  if (m_mode == Core::Mode::LiangBarsky) {
    switch (index) {
    case 0:
      m_win = {3.0, 2.0, 7.0, 5.0};
      m_segs = {{{2.0, 1.0}, {8.0, 6.0}}};
      break;
    case 1:
      m_win = {2.0, 2.0, 8.0, 6.0};
      m_segs = {{{1.0, 3.0}, {4.2, 6.8}},
                {{3.2, 2.8}, {6.0, 4.6}},
                {{5.2, 1.0}, {9.0, 4.2}},
                {{0.4, 1.2}, {1.2, 4.8}},
                {{4.8, 6.0}, {7.4, 6.0}}};
      break;
    case 2:
      m_win = {-4.0, -3.0, 4.0, 3.0};
      m_segs = {{{-6.0, -4.5}, {6.0, 4.5}},
                {{-6.0, 4.5}, {6.0, -4.5}},
                {{-6.0, 0.0}, {6.0, 0.0}},
                {{0.0, -5.0}, {0.0, 5.0}},
                {{-5.0, 2.0}, {2.0, -4.5}}};
      break;
    case 3:
    default:
      m_segs.clear();
      break;
    }
  } else {
    switch (index) {
    case 0:
      m_win = {3.0, 2.0, 7.0, 5.0};
      m_poly = {{1.5, 3.5}, {4.5, 6.2}, {8.2, 4.5}, {6.5, 1.0}, {2.5, 1.2}};
      break;
    case 1:
      m_win = {2.0, 2.0, 8.0, 6.0};
      m_poly = {{3.0, 2.6}, {6.2, 2.4}, {9.2, 4.4},
                {6.8, 7.2}, {3.2, 6.8}, {2.0, 4.4}};
      break;
    case 2:
      m_win = {2.0, 2.0, 8.0, 6.0};
      m_poly = {{5.0, 0.5}, {9.5, 4.0}, {5.0, 7.5}, {0.5, 4.0}};
      break;
    case 3:
    default:
      m_poly.clear();
      break;
    }
  }

  emit stateSynced();
  recalc();
  emit fitRequested();
  emit statusChanged("Preset applied", 0);
}
void Controller::load(const QString &path) {
  Core::FileData data;
  std::string err;
  if (!Core::Io::loadFile(path.toStdString(), data, err)) {
    emit statusChanged(QString::fromStdString(err), 2);
    return;
  }

  m_win = data.win;
  if (!data.segs.empty())
    m_segs = data.segs;
  if (!data.poly.empty())
    m_poly = data.poly;
  if (data.hasPoly)
    m_mode = Core::Mode::SutherlandHodgman;

  m_hasSel = false;
  emit stateSynced();
  recalc();
  emit fitRequested();
  emit statusChanged("Loaded: " + path, 1);
}

void Controller::save(const QString &path) {
  std::string err;
  if (Core::Io::saveFile(path.toStdString(), m_mode, m_win, m_segs, m_poly,
                         err)) {
    emit statusChanged("Saved: " + path, 1);
  } else {
    emit statusChanged(QString::fromStdString(err), 2);
  }
}

void Controller::setGrid(bool v) {
  if (m_grid == v)
    return;
  m_grid = v;
  emit viewChanged();
}

void Controller::setAxes(bool v) {
  if (m_axes == v)
    return;
  m_axes = v;
  emit viewChanged();
}

void Controller::setWinLayer(bool v) {
  if (m_winLayer == v)
    return;
  m_winLayer = v;
  emit viewChanged();
}

void Controller::setOrigLayer(bool v) {
  if (m_origLayer == v)
    return;
  m_origLayer = v;
  emit viewChanged();
}

void Controller::setClipLayer(bool v) {
  if (m_clipLayer == v)
    return;
  m_clipLayer = v;
  emit viewChanged();
}

void Controller::setLabels(bool v) {
  if (m_labels == v)
    return;
  m_labels = v;
  emit viewChanged();
}

void Controller::selectStep(int idx) {
  if (idx >= 0 && idx < static_cast<int>(m_res.steps.size())) {
    m_hasSel = true;
    m_selX = m_res.steps[idx].x;
    m_selY = m_res.steps[idx].y;
    emit selChanged(m_selX, m_selY, true);
  }
}

void Controller::clearSel() {
  if (!m_hasSel)
    return;
  m_hasSel = false;
  emit selChanged(0.0, 0.0, false);
}

void Controller::recalc() {
  if (m_mode == Core::Mode::LiangBarsky) {
    m_res = Core::Ops::liangBarsky(m_win, m_segs, true);
  } else {
    m_res = Core::Ops::sutherlandHodgman(m_win, m_poly, true);
  }
  emit resultChanged(m_res);
}
