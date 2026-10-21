#include "Hist.h"
#include "../common/Combo.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <algorithm>

HistCanvas::HistCanvas(QWidget *parent) : QWidget(parent) {
  setObjectName("histCanvas");
  setFixedHeight(110);
}

void HistCanvas::setData(const HistData &data) {
  m_data = data;
  update();
}

void HistCanvas::setMode(int mode) {
  m_mode = mode;
  update();
}

void HistCanvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.fillRect(rect(), QColor("#0c0d10"));

  const double w = static_cast<double>(width() - 2);
  const double h = static_cast<double>(height() - 2);
  const double bottom = h + 1.0;
  const double penW = std::max(1.0, w / 255.0);

  uint32_t maxVal = 1;
  if (m_mode == 0) {
    for (int i = 0; i < 256; ++i) {
      maxVal = std::max({maxVal, m_data.r[i], m_data.g[i], m_data.b[i]});
    }
  } else {
    const auto &ch = (m_mode == 1) ? m_data.gray :
                     (m_mode == 2) ? m_data.r :
                     (m_mode == 3) ? m_data.g : m_data.b;
    for (int i = 0; i < 256; ++i) {
      maxVal = std::max(maxVal, ch[i]);
    }
  }

  const double scale = (maxVal > 0) ? (h - 2.0) / (maxVal * 1.02) : 1.0;

  auto drawChannelBars = [&](const std::vector<uint32_t> &counts, const QColor &col) {
    p.setPen(QPen(col, penW));
    for (int i = 0; i < 256; ++i) {
      if (counts[i] == 0) continue;
      const double x = 1.0 + (i / 255.0) * w;
      const double barH = counts[i] * scale;
      p.drawLine(QPointF(x, bottom), QPointF(x, bottom - barH));
    }
  };

  if (m_mode == 0) {
    p.setCompositionMode(QPainter::CompositionMode_Plus);
    drawChannelBars(m_data.r, QColor(220, 45, 45, 200));
    drawChannelBars(m_data.g, QColor(35, 195, 80, 200));
    drawChannelBars(m_data.b, QColor(45, 120, 245, 200));
  } else if (m_mode == 1) {
    drawChannelBars(m_data.gray, QColor(203, 213, 225));
  } else if (m_mode == 2) {
    drawChannelBars(m_data.r, QColor(239, 68, 68));
  } else if (m_mode == 3) {
    drawChannelBars(m_data.g, QColor(34, 197, 94));
  } else if (m_mode == 4) {
    drawChannelBars(m_data.b, QColor(59, 130, 246));
  }
}

Hist::Hist(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(6);

  auto *topRow = new QHBoxLayout();
  topRow->setContentsMargins(0, 0, 0, 0);

  auto *title = new QLabel("HISTOGRAM", this);
  title->setObjectName("sectionTitle");

  auto *combo = new Combo(this);
  combo->addItem("Colors");
  combo->addItem("Luminance");
  combo->addItem("Red");
  combo->addItem("Green");
  combo->addItem("Blue");

  topRow->addWidget(title);
  topRow->addStretch(1);
  topRow->addWidget(combo);
  layout->addLayout(topRow);

  m_canvas = new HistCanvas(this);
  layout->addWidget(m_canvas);

  m_stats = new QLabel("Min: 0  Max: 255  Avg: 0.0", this);
  m_stats->setObjectName("statsLabel");
  layout->addWidget(m_stats);

  connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          m_canvas, &HistCanvas::setMode);
}

void Hist::setData(const HistData &data) {
  m_canvas->setData(data);
  m_stats->setText(QString("Min: %1  Max: %2  Avg: %3")
                       .arg(data.stats.min)
                       .arg(data.stats.max)
                       .arg(data.stats.avg, 0, 'f', 1));
}