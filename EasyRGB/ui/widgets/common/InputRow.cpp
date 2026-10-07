#include "InputRow.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

class CopyBtn : public QPushButton {
public:
  explicit CopyBtn(QWidget *parent = nullptr) : QPushButton(parent) {
    setObjectName("rowCopyBtn");
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
  }

  void setCopied(bool state) {
    m_copied = state;
    update();
  }

protected:
  void paintEvent(QPaintEvent *e) override {
    QPushButton::paintEvent(e);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_copied) {
      p.setPen(QPen(QColor("#10b981"), 1.8, Qt::SolidLine, Qt::RoundCap,
                    Qt::RoundJoin));
      p.drawLine(8, 14, 12, 18);
      p.drawLine(12, 18, 20, 9);
    } else {
      p.setPen(QPen(underMouse() ? QColor("#ffffff") : QColor("#8b949e"), 1.4));
      p.setBrush(Qt::NoBrush);
      p.drawRoundedRect(11, 7, 10, 12, 1.5, 1.5);
      p.fillRect(7, 10, 10, 12,
                 underMouse() ? QColor("#262930") : QColor("#1b1d22"));
      p.drawRoundedRect(7, 10, 10, 12, 1.5, 1.5);
    }
  }

private:
  bool m_copied{false};
};

InputRow::InputRow(const QString &label, QWidget *parent) : QWidget(parent) {
  setFixedHeight(30);

  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(6);

  m_label = new QLabel(label, this);
  m_label->setObjectName("rowLabel");
  m_label->setFixedWidth(38);

  m_input = new QLineEdit(this);
  m_input->setObjectName("rowInput");
  m_input->setFocusPolicy(Qt::ClickFocus);

  m_copyBtn = new CopyBtn(this);

  layout->addWidget(m_label);
  layout->addWidget(m_input, 1);
  layout->addWidget(m_copyBtn);

  connect(m_input, &QLineEdit::returnPressed, this,
          [this]() { emit submitted(m_input->text()); });
  connect(m_copyBtn, &QPushButton::clicked, this, [this]() {
    QGuiApplication::clipboard()->setText(m_input->text());
    m_copyBtn->setCopied(true);
    QTimer::singleShot(1000, this, [this]() { m_copyBtn->setCopied(false); });
  });
}

void InputRow::setValue(const QString &val) {
  const QSignalBlocker b(m_input);
  m_input->setText(val);
}

QString InputRow::value() const { return m_input->text(); }
void InputRow::setPlaceholder(const QString &holder) {
  m_input->setPlaceholderText(holder);
}
void InputRow::setValidator(const QValidator *val) {
  m_input->setValidator(val);
}
