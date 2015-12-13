#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include <boost/signals2.hpp>
#include "ui_main_window.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow();

  boost::signals2::signal<void (const std::string& target, const std::string& rules)> onRequestScan;

  void getUserInput();

private:

  Ui::MainWindow m_ui;

};

#endif // __MAIN_WINDOW_H__
