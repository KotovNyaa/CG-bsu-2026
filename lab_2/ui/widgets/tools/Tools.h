#pragma once

#include <QFrame>

class Hist;
class Contrast;
class Filter;
class QPushButton;

class Tools : public QFrame {
  Q_OBJECT

public:
  explicit Tools(QWidget *parent = nullptr);

  Hist *hist() const { return m_hist; }
  Contrast *contrast() const { return m_contrast; }
  Filter *filter() const { return m_filter; }

signals:
  void commitRequested();
  void discardRequested();

private:
  Hist *m_hist{nullptr};
  Contrast *m_contrast{nullptr};
  Filter *m_filter{nullptr};
};