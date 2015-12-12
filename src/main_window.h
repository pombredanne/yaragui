#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include <boost/signals2.hpp>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow();

  boost::signals2::signal<void (const std::string& target, const std::string& rules)> onRequestScan;

  void getUserInput();

private:

};

#endif // __MAIN_WINDOW_H__
