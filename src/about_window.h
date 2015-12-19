#ifndef __ABOUT_WINDOW_H__
#define __ABOUT_WINDOW_H__

#include "ui_about_window.h"

class AboutWindow : public QMainWindow
{
  Q_OBJECT

public:

  AboutWindow();

private:

  Ui::AboutWindow m_ui;

};

#endif // __ABOUT_WINDOW_H__
