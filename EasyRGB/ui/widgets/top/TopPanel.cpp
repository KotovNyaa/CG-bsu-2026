#include "TopPanel.h"
#include "../common/InputRow.h"
#include "CieDialog.h"
#include "Picker2D.h"
#include "PreviewBox.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

TopPanel::TopPanel(QWidget *parent) : QFrame(parent) {
  setObjectName("topPanel");
  setFixedHeight(386);

  auto *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setSpacing(16);

  m_picker = new Picker2D(this);
  mainLayout->addWidget(m_picker);

  auto *info = new QWidget(this);
  info->setFixedSize(588, 350);

  auto *infoLayout = new QVBoxLayout(info);
  infoLayout->setContentsMargins(0, 0, 0, 0);
  infoLayout->setSpacing(10);

  auto *topControls = new QWidget(info);
  topControls->setFixedHeight(72);

  auto *topControlsLayout = new QHBoxLayout(topControls);
  topControlsLayout->setContentsMargins(0, 0, 0, 0);
  topControlsLayout->setSpacing(12);

  m_preview = new PreviewBox(topControls);

  auto *actions = new QWidget(topControls);
  auto *actionsLayout = new QVBoxLayout(actions);
  actionsLayout->setContentsMargins(0, 2, 0, 2);
  actionsLayout->setSpacing(8);

  auto *headerRow = new QWidget(actions);
  auto *headerLayout = new QHBoxLayout(headerRow);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  headerLayout->setSpacing(0);

  auto *title = new QLabel("Lab 1: Convert color data", headerRow);
  title->setObjectName("titleLabel");

  m_mkoBtn = new QPushButton("MKO", headerRow);
  m_mkoBtn->setObjectName("mkoBtn");
  m_mkoBtn->setFixedSize(80, 28);
  m_mkoBtn->setCursor(Qt::PointingHandCursor);

  headerLayout->addWidget(title);
  headerLayout->addStretch(1);
  headerLayout->addWidget(m_mkoBtn);

  m_hexRow = new InputRow("HEX", actions);
  m_hexRow->setPlaceholder("#RRGGBB");
  m_hexRow->setValidator(new QRegularExpressionValidator(
      QRegularExpression("^#?[0-9A-Fa-f]{6}$"), m_hexRow));

  actionsLayout->addWidget(headerRow);
  actionsLayout->addWidget(m_hexRow);

  topControlsLayout->addWidget(m_preview);
  topControlsLayout->addWidget(actions, 1);

  m_console = new QPlainTextEdit(info);
  m_console->setObjectName("dataConsole");
  m_console->setReadOnly(true);
  m_console->setFixedHeight(268);

  infoLayout->addWidget(topControls);
  infoLayout->addWidget(m_console);

  mainLayout->addWidget(info, 1);

  m_cieDialog = new CieDialog(this);

  connect(m_mkoBtn, &QPushButton::clicked, this, [this]() {
    m_cieDialog->show();
    m_cieDialog->raise();
    m_cieDialog->activateWindow();
  });

  connect(m_picker, &Picker2D::hsvChanged, this,
          [this](double h, double s, double v) {
            emit hsvChanged(h, s, v, m_picker);
          });
  connect(m_hexRow, &InputRow::submitted, this,
          [this](const QString &hex) { emit hexSubmitted(hex, m_hexRow); });
}

void TopPanel::setPreviewColor(const QColor &color) {
  m_preview->setColor(color);
}

void TopPanel::setRgb(const ColorLib::Rgb &rgb) {
  setPreviewColor(QColor::fromRgbF(rgb.r, rgb.g, rgb.b));
  if (m_cieDialog) {
    m_cieDialog->setRgb(rgb);
  }
}

void TopPanel::setHex(const QString &hex, QObject *origin) {
  if (origin != m_hexRow)
    m_hexRow->setValue(hex);
}

void TopPanel::setHsv(double h, double s, double v, QObject *origin) {
  if (origin != m_picker)
    m_picker->setHsv(h, s, v);
}

void TopPanel::setReport(const QString &report) {
  m_console->setPlainText(report);
}
