#include "Slider.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>

class ClickSlider : public QSlider {
public:
  explicit ClickSlider(Qt::Orientation ori, QWidget *parent = nullptr) : QSlider(ori, parent) {}

protected:
  void mousePressEvent(QMouseEvent *e) override {
    if (e->button() == Qt::LeftButton) {
      setValue(QStyle::sliderValueFromPosition(minimum(), maximum(), e->pos().x(), width()));
      e->accept();
    }
    QSlider::mousePressEvent(e);
  }
};

Slider::Slider(const QString &label, int min, int max, int val, QWidget *parent)
    : QWidget(parent) {
  setFixedHeight(28);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);

  auto *lbl = new QLabel(label, this);
  lbl->setObjectName("sliderLabel");
  lbl->setFixedWidth(30);

  m_slider = new ClickSlider(Qt::Horizontal, this);
  m_slider->setRange(min, max);
  m_slider->setValue(val);
  m_slider->setFocusPolicy(Qt::NoFocus);

  m_spin = new QSpinBox(this);
  m_spin->setRange(min, max);
  m_spin->setValue(val);
  m_spin->setFixedWidth(52);
  m_spin->setFocusPolicy(Qt::ClickFocus);

  layout->addWidget(lbl);
  layout->addWidget(m_slider, 1);
  layout->addWidget(m_spin);

  connect(m_slider, &QSlider::valueChanged, m_spin, &QSpinBox::setValue);
  connect(m_spin, QOverload<int>::of(&QSpinBox::valueChanged), m_slider, &QSlider::setValue);
  connect(m_slider, &QSlider::valueChanged, this, &Slider::valueChanged);
}

void Slider::setValue(int val) {
  m_slider->setValue(val);
}

int Slider::value() const {
  return m_slider->value();
}