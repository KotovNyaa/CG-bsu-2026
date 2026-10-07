#include "CieDialog.h"
#include "../../../lib/ColorMath.h"
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>

static const QVector<QPointF> s_locusPoints = {
    {0.1741, 0.0050}, {0.1738, 0.0049}, {0.1733, 0.0048}, {0.1726, 0.0048},
    {0.1714, 0.0051}, {0.1689, 0.0063}, {0.1644, 0.0109}, {0.1566, 0.0177},
    {0.1440, 0.0297}, {0.1241, 0.0578}, {0.1096, 0.0868}, {0.0913, 0.1327},
    {0.0687, 0.2007}, {0.0454, 0.2950}, {0.0235, 0.4127}, {0.0082, 0.5384},
    {0.0039, 0.6548}, {0.0139, 0.7502}, {0.0389, 0.8120}, {0.0743, 0.8338},
    {0.1142, 0.8262}, {0.1547, 0.8059}, {0.1929, 0.7816}, {0.2296, 0.7543},
    {0.2658, 0.7243}, {0.3016, 0.6923}, {0.3373, 0.6589}, {0.3731, 0.6245},
    {0.4087, 0.5896}, {0.4441, 0.5547}, {0.4788, 0.5202}, {0.5125, 0.4866},
    {0.5448, 0.4544}, {0.5752, 0.4242}, {0.6029, 0.3965}, {0.6270, 0.3725},
    {0.6482, 0.3514}, {0.6658, 0.3340}, {0.6801, 0.3197}, {0.6915, 0.3083},
    {0.7079, 0.2920}, {0.7190, 0.2809}, {0.7260, 0.2740}, {0.7300, 0.2700},
    {0.7320, 0.2680}, {0.7334, 0.2666}, {0.7347, 0.2653}};

CieCanvas::CieCanvas(QWidget *parent) : QWidget(parent) {
  setFixedSize(380, 420);
  renderBackground();
}

QPointF CieCanvas::toCanvas(double x, double y) const {
  const double drawW = width() - 2 * m_padding;
  const double drawH = height() - 2 * m_padding;
  const double px = m_padding + ((x - m_minX) / (m_maxX - m_minX)) * drawW;
  const double py =
      height() - m_padding - ((y - m_minY) / (m_maxY - m_minY)) * drawH;
  return {px, py};
}

void CieCanvas::renderBackground() {
  m_bgCache = QPixmap(size());
  m_bgCache.fill(QColor("#0f1013"));

  QPolygonF locusPoly;
  for (const auto &pt : s_locusPoints) {
    locusPoly.append(toCanvas(pt.x(), pt.y()));
  }

  QPainterPath locusPath;
  locusPath.moveTo(locusPoly.first());
  for (int i = 1; i < locusPoly.size(); ++i) {
    locusPath.lineTo(locusPoly[i]);
  }
  locusPath.closeSubpath();

  QImage spectrumImg(size(), QImage::Format_ARGB32_Premultiplied);
  spectrumImg.fill(Qt::transparent);

  const double drawW = width() - 2 * m_padding;
  const double drawH = height() - 2 * m_padding;
  const QRect bounds = locusPath.boundingRect().toRect();

  for (int py = bounds.top(); py <= bounds.bottom(); ++py) {
    const double y =
        m_minY + ((height() - m_padding - py) / drawH) * (m_maxY - m_minY);
    if (y <= 0.002)
      continue;

    for (int px = bounds.left(); px <= bounds.right(); ++px) {
      if (!locusPath.contains(QPointF(px, py)))
        continue;

      const double x = m_minX + ((px - m_padding) / drawW) * (m_maxX - m_minX);
      const double z = 1.0 - x - y;
      if (z < -0.05)
        continue;

      const double X = x / y;
      const double Y = 1.0;
      const double Z = std::max(0.0, z / y);

      double r = 3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z;
      double g = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z;
      double b = 0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z;

      r = std::max(0.0, r);
      g = std::max(0.0, g);
      b = std::max(0.0, b);
      const double maxVal = std::max({r, g, b});
      if (maxVal > 0.0) {
        r /= maxVal;
        g /= maxVal;
        b /= maxVal;
      }

      r = std::clamp(std::pow(r, 1.0 / 2.2), 0.0, 1.0);
      g = std::clamp(std::pow(g, 1.0 / 2.2), 0.0, 1.0);
      b = std::clamp(std::pow(b, 1.0 / 2.2), 0.0, 1.0);

      spectrumImg.setPixelColor(px, py,
                                QColor(static_cast<int>(r * 255.0),
                                       static_cast<int>(g * 255.0),
                                       static_cast<int>(b * 255.0), 220));
    }
  }

  QPainter p(&m_bgCache);
  p.setRenderHint(QPainter::Antialiasing);

  p.drawImage(0, 0, spectrumImg);

  p.setPen(QPen(QColor("#f8fafc"), 1.8));
  p.drawPath(locusPath);

  QFont font = p.font();
  font.setPixelSize(10);
  p.setFont(font);

  for (double x = 0.0; x <= 0.801; x += 0.1) {
    const double px = m_padding + (x / 0.8) * drawW;
    p.setPen(QPen(QColor(255, 255, 255, 35), 1, Qt::DotLine));
    p.drawLine(QPointF(px, m_padding), QPointF(px, height() - m_padding));
    p.setPen(QColor("#94a3b8"));
    p.drawText(QRectF(px - 14, height() - m_padding + 4, 28, 16),
               Qt::AlignCenter, QString::number(x, 'f', 1));
  }

  for (double y = 0.0; y <= 0.901; y += 0.1) {
    const double py = height() - m_padding - (y / 0.9) * drawH;
    p.setPen(QPen(QColor(255, 255, 255, 35), 1, Qt::DotLine));
    p.drawLine(QPointF(m_padding, py), QPointF(width() - m_padding, py));
    p.setPen(QColor("#94a3b8"));
    p.drawText(QRectF(2, py - 8, m_padding - 6, 16),
               Qt::AlignRight | Qt::AlignVCenter, QString::number(y, 'f', 1));
  }

  p.setPen(QPen(QColor("#64748b"), 1.5));
  p.drawLine(m_padding, height() - m_padding, width() - m_padding,
             height() - m_padding);
  p.drawLine(m_padding, m_padding, m_padding, height() - m_padding);

  const QPointF rPt = toCanvas(0.64, 0.33);
  const QPointF gPt = toCanvas(0.30, 0.60);
  const QPointF bPt = toCanvas(0.15, 0.06);

  QPolygonF srgbPoly;
  srgbPoly << rPt << gPt << bPt;

  p.setPen(QPen(Qt::white, 2.0, Qt::SolidLine));
  p.drawPolygon(srgbPoly);

  const QPointF d65 = toCanvas(0.3127, 0.3290);
  p.setPen(QPen(Qt::black, 2));
  p.drawLine(QPointF(d65.x() - 4, d65.y()), QPointF(d65.x() + 4, d65.y()));
  p.drawLine(QPointF(d65.x(), d65.y() - 4), QPointF(d65.x(), d65.y() + 4));
  p.setPen(QPen(Qt::white, 1));
  p.drawLine(QPointF(d65.x() - 4, d65.y()), QPointF(d65.x() + 4, d65.y()));
  p.drawLine(QPointF(d65.x(), d65.y() - 4), QPointF(d65.x(), d65.y() + 4));

  auto drawTextWithShadow = [&](const QPointF &pt, const QString &txt) {
    p.setPen(Qt::black);
    p.drawText(pt + QPointF(1, 1), txt);
    p.setPen(Qt::white);
    p.drawText(pt, txt);
  };

  drawTextWithShadow(rPt + QPointF(8, 4), "R");
  drawTextWithShadow(gPt + QPointF(-4, -8), "G");
  drawTextWithShadow(bPt + QPointF(-14, 12), "B");
  drawTextWithShadow(d65 + QPointF(7, -4), "D65");
}

void CieCanvas::setColor(const ColorLib::Rgb &rgb, double x, double y) {
  m_currentRgb = rgb;
  m_curX = std::clamp(x, m_minX, m_maxX);
  m_curY = std::clamp(y, m_minY, m_maxY);
  update();
}

void CieCanvas::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.drawPixmap(0, 0, m_bgCache);

  p.setRenderHint(QPainter::Antialiasing);
  const QPointF pos = toCanvas(m_curX, m_curY);

  p.setPen(QPen(QColor(255, 255, 255, 160), 1, Qt::DashLine));
  p.drawLine(QPointF(pos.x(), m_padding),
             QPointF(pos.x(), height() - m_padding));
  p.drawLine(QPointF(m_padding, pos.y()),
             QPointF(width() - m_padding, pos.y()));

  p.setPen(QPen(Qt::black, 2.5));
  p.setBrush(QColor::fromRgbF(m_currentRgb.r, m_currentRgb.g, m_currentRgb.b));
  p.drawEllipse(pos, 6.5, 6.5);

  p.setPen(QPen(Qt::white, 1.2));
  p.setBrush(Qt::NoBrush);
  p.drawEllipse(pos, 6.5, 6.5);
}

CieDialog::CieDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle("CIE 1931 xy — График МКО");
  setFixedSize(412, 510);
  setStyleSheet("background-color: #16171b; color: #f0f6fc;");

  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setSpacing(12);

  m_canvas = new CieCanvas(this);
  mainLayout->addWidget(m_canvas, 0, Qt::AlignCenter);

  auto *bottomBox = new QWidget(this);
  bottomBox->setStyleSheet("background-color: #0f1013; border: 1px solid "
                           "#23252b; border-radius: 6px;");
  bottomBox->setFixedHeight(46);

  auto *bottomLayout = new QHBoxLayout(bottomBox);
  bottomLayout->setContentsMargins(10, 6, 10, 6);
  bottomLayout->setSpacing(10);

  m_swatch = new QWidget(bottomBox);
  m_swatch->setFixedSize(30, 30);
  m_swatch->setStyleSheet("border: 1px solid #3b82f6; border-radius: 4px;");

  m_coordsLabel = new QLabel(bottomBox);
  m_coordsLabel->setStyleSheet(
      "border: none; font-family: monospace; font-size: 12px;");

  bottomLayout->addWidget(m_swatch);
  bottomLayout->addWidget(m_coordsLabel, 1);

  mainLayout->addWidget(bottomBox);
}

void CieDialog::setRgb(const ColorLib::Rgb &rgb) {
  double x = 0.3127, y = 0.3290;
  ColorLib::ColorMath::rgbToXy(rgb, x, y);
  const double z = std::max(0.0, 1.0 - x - y);

  m_canvas->setColor(rgb, x, y);

  m_swatch->setStyleSheet(
      QString("background-color: rgb(%1, %2, %3); border: 1px solid #475569; "
              "border-radius: 4px;")
          .arg(static_cast<int>(std::round(rgb.r * 255.0)))
          .arg(static_cast<int>(std::round(rgb.g * 255.0)))
          .arg(static_cast<int>(std::round(rgb.b * 255.0))));

  m_coordsLabel->setText(QString("x: %1   y: %2   z: %3")
                             .arg(x, 6, 'f', 4)
                             .arg(y, 6, 'f', 4)
                             .arg(z, 6, 'f', 4));
}
