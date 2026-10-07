#pragma once

#include "../../../lib/ColorTypes.h"
#include <QFrame>

class Picker2D;
class PreviewBox;
class InputRow;
class QPlainTextEdit;
class QPushButton;
class CieDialog;

class TopPanel : public QFrame {
  Q_OBJECT

public:
  explicit TopPanel(QWidget *parent = nullptr);

  void setPreviewColor(const QColor &color);
  void setRgb(const ColorLib::Rgb &rgb);
  void setHex(const QString &hex, QObject *origin);
  void setHsv(double h, double s, double v, QObject *origin);
  void setReport(const QString &report);

  Picker2D *picker() const { return m_picker; }
  InputRow *hexRow() const { return m_hexRow; }

signals:
  void hexSubmitted(const QString &hex, QObject *origin);
  void hsvChanged(double h, double s, double v, QObject *origin);

private:
  Picker2D *m_picker{nullptr};
  PreviewBox *m_preview{nullptr};
  InputRow *m_hexRow{nullptr};
  QPushButton *m_mkoBtn{nullptr};
  QPlainTextEdit *m_console{nullptr};
  CieDialog *m_cieDialog{nullptr};
};
