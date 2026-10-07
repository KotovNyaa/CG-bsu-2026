#include "Picker2D.h"
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <algorithm>

class SvCanvas : public QWidget {
  Q_OBJECT

public:
  explicit SvCanvas(QWidget *parent = nullptr) : QWidget(parent) {
    setObjectName("svCanvas");
    setFixedSize(350, 350);
    setCursor(Qt::CrossCursor);
  }

  void setHue(double h) {
    m_h = h;
    update();
  }

  void setSv(double s, double v) {
    m_s = std::clamp(s, 0.0, 1.0);
    m_v = std::clamp(v, 0.0, 1.0);
    update();
  }

signals:
  void svChanged(double s, double v);

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    p.fillRect(rect(), QColor::fromHsvF(m_h / 360.0, 1.0, 1.0));

    QLinearGradient whiteGrad(0, 0, width(), 0);
    whiteGrad.setColorAt(0.0, QColor(255, 255, 255, 255));
    whiteGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.fillRect(rect(), whiteGrad);

    QLinearGradient blackGrad(0, 0, 0, height());
    blackGrad.setColorAt(0.0, QColor(0, 0, 0, 0));
    blackGrad.setColorAt(1.0, QColor(0, 0, 0, 255));
    p.fillRect(rect(), blackGrad);

    const int px = static_cast<int>(m_s * (width() - 1));
    const int py = static_cast<int>((1.0 - m_v) * (height() - 1));

    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::white, 2));
    p.drawEllipse(QPoint(px, py), 5, 5);
    p.setPen(QPen(Qt::black, 1));
    p.drawEllipse(QPoint(px, py), 6, 6);
  }

  void mousePressEvent(QMouseEvent *e) override { updatePos(e->pos()); }
  void mouseMoveEvent(QMouseEvent *e) override {
    if (e->buttons() & Qt::LeftButton) updatePos(e->pos());
  }

private:
  void updatePos(const QPoint &pos) {
    m_s = std::clamp(static_cast<double>(pos.x()) / (width() - 1), 0.0, 1.0);
    m_v = std::clamp(1.0 - static_cast<double>(pos.y()) / (height() - 1), 0.0, 1.0);
    update();
    emit svChanged(m_s, m_v);
  }

  double m_h{0.0};
  double m_s{1.0};
  double m_v{1.0};
};

class HueBar : public QWidget {
  Q_OBJECT

public:
  explicit HueBar(QWidget *parent = nullptr) : QWidget(parent) {
    setObjectName("hueBar");
    setFixedSize(26, 350);
    setCursor(Qt::SizeVerCursor);
  }

  void setHue(double h) {
    m_h = std::clamp(h, 0.0, 360.0);
    update();
  }

signals:
  void hueChanged(double h);

protected:
  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0.0 / 6.0, Qt::red);
    grad.setColorAt(1.0 / 6.0, Qt::yellow);
    grad.setColorAt(2.0 / 6.0, Qt::green);
    grad.setColorAt(3.0 / 6.0, Qt::cyan);
    grad.setColorAt(4.0 / 6.0, Qt::blue);
    grad.setColorAt(5.0 / 6.0, Qt::magenta);
    grad.setColorAt(6.0 / 6.0, Qt::red);
    p.fillRect(rect(), grad);

    const int py = static_cast<int>((m_h / 360.0) * (height() - 1));
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(0, py, width(), py);
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(0, std::max(0, py - 1), width() - 1, 2);
  }

  void mousePressEvent(QMouseEvent *e) override { updatePos(e->pos()); }
  void mouseMoveEvent(QMouseEvent *e) override {
    if (e->buttons() & Qt::LeftButton) updatePos(e->pos());
  }

private:
  void updatePos(const QPoint &pos) {
    m_h = std::clamp(static_cast<double>(pos.y()) / (height() - 1) * 360.0, 0.0, 360.0);
    update();
    emit hueChanged(m_h);
  }

  double m_h{0.0};
};

Picker2D::Picker2D(QWidget *parent) : QWidget(parent) {
  setFixedSize(384, 350);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  m_svCanvas = new SvCanvas(this);
  m_hueBar = new HueBar(this);

  layout->addWidget(m_svCanvas);
  layout->addWidget(m_hueBar);

  connect(m_svCanvas, &SvCanvas::svChanged, this, [this](double s, double v) {
    m_s = s;
    m_v = v;
    emit hsvChanged(m_h, m_s, m_v);
  });

  connect(m_hueBar, &HueBar::hueChanged, this, [this](double h) {
    m_h = h;
    m_svCanvas->setHue(m_h);
    emit hsvChanged(m_h, m_s, m_v);
  });
}

void Picker2D::setHsv(double h, double s, double v) {
  m_h = std::clamp(h, 0.0, 360.0);
  m_s = std::clamp(s, 0.0, 1.0);
  m_v = std::clamp(v, 0.0, 1.0);
  m_svCanvas->setHue(m_h);
  m_svCanvas->setSv(m_s, m_v);
  m_hueBar->setHue(m_h);
}

#include "Picker2D.moc"