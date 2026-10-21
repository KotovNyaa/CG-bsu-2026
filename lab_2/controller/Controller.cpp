#include "Controller.h"
#include "../core/Ops.h"

Controller::Controller(QObject *parent) : QObject(parent) {
  makeStubImage();
  updatePipeline();
}

void Controller::makeStubImage() {
  m_original = QImage(512, 512, QImage::Format_RGB888);
  for (int y = 0; y < 512; ++y) {
    auto *line = m_original.scanLine(y);
    const double fy = y / 511.0;
    for (int x = 0; x < 512; ++x) {
      const double fx = x / 511.0;
      const double rVal = fx * 255.0;
      const double gVal = fy * 255.0;
      const double bVal = (1.0 - 0.5 * (fx + fy)) * 255.0;
      line[x * 3] = static_cast<uchar>(std::clamp(std::round(rVal), 0.0, 255.0));
      line[x * 3 + 1] = static_cast<uchar>(std::clamp(std::round(gVal), 0.0, 255.0));
      line[x * 3 + 2] = static_cast<uchar>(std::clamp(std::round(bVal), 0.0, 255.0));
    }
  }
  m_base = m_original;
  m_current = m_base;
}

void Controller::updatePipeline() {
  QImage processed = m_base;

  if (m_contrastMode == 1) {
    processed = Ops::linearStretch(processed, m_minVal, m_maxVal);
  } else if (m_contrastMode == 2) {
    processed = Ops::equalizeRgb(processed);
  } else if (m_contrastMode == 3) {
    processed = Ops::equalizeHls(processed);
  }

  if (m_filterType > 0) {
    processed = Ops::applyFilter(processed, m_filterType, m_radius);
  }

  m_current = processed;
  Ops::computeHist(m_current, m_hist);

  emit imageChanged(m_current);
  emit histChanged(m_hist);
  emit statusChanged("Ready", 0);
}

void Controller::load(const QString &path) {
  QImage img;
  if (img.load(path)) {
    m_original = img.convertToFormat(QImage::Format_RGB888);
    m_base = m_original;
    m_contrastMode = 0;
    m_filterType = 0;
    m_minVal = 0;
    m_maxVal = 255;
    emit stateReset();
    updatePipeline();
    emit statusChanged("Loaded: " + path, 1);
  } else {
    emit statusChanged("Failed to load file", 2);
  }
}

void Controller::save(const QString &path) {
  if (m_current.save(path)) emit statusChanged("Saved: " + path, 1);
  else emit statusChanged("Failed to save file", 2);
}

void Controller::resetOriginal() {
  m_base = m_original;
  m_contrastMode = 0;
  m_filterType = 0;
  m_minVal = 0;
  m_maxVal = 255;
  emit stateReset();
  updatePipeline();
  emit statusChanged("Reset to original image", 0);
}

void Controller::showOriginal(bool original) {
  if (original) {
    HistData origHist;
    Ops::computeHist(m_original, origHist);
    emit imageChanged(m_original);
    emit histChanged(origHist);
    emit statusChanged("Viewing original", 0);
  } else {
    emit imageChanged(m_current);
    emit histChanged(m_hist);
    emit statusChanged("Viewing preview", 0);
  }
}

void Controller::setContrastMode(int mode) {
  m_contrastMode = mode;
  updatePipeline();
}

void Controller::setContrastRange(int minVal, int maxVal) {
  m_minVal = minVal;
  m_maxVal = maxVal;
  m_contrastMode = 1;
  updatePipeline();
}

void Controller::autoContrast() {
  HistData baseHist;
  Ops::computeHist(m_base, baseHist);
  m_minVal = baseHist.stats.min;
  m_maxVal = baseHist.stats.max;
  m_contrastMode = 1;
  emit contrastRangeCalculated(m_minVal, m_maxVal);
  updatePipeline();
}

void Controller::setFilter(int type, int radius) {
  m_filterType = type;
  m_radius = radius;
  updatePipeline();
}

void Controller::commit() {
  m_base = m_current;
  m_contrastMode = 0;
  m_filterType = 0;
  m_minVal = 0;
  m_maxVal = 255;
  emit stateReset();
  emit statusChanged("Changes committed to base", 1);
}

void Controller::discard() {
  m_contrastMode = 0;
  m_filterType = 0;
  m_minVal = 0;
  m_maxVal = 255;
  emit stateReset();
  updatePipeline();
  emit statusChanged("Preview discarded", 0);
}