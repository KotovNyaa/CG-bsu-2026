#pragma once

#include <QString>
#include <QWidget>

class QLabel;
class QLineEdit;
class QValidator;
class CopyBtn;

class InputRow : public QWidget {
  Q_OBJECT

public:
  explicit InputRow(const QString &label, QWidget *parent = nullptr);

  void setValue(const QString &val);
  QString value() const;

  void setPlaceholder(const QString &holder);
  void setValidator(const QValidator *val);

signals:
  void submitted(const QString &val);

private:
  QLabel *m_label{nullptr};
  QLineEdit *m_input{nullptr};
  CopyBtn *m_copyBtn{nullptr};
};