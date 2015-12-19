#ifndef __RULE_WINDOW_H__
#define __RULE_WINDOW_H__

#include "ui_rule_window.h"

class RuleWindow : public QMainWindow
{
  Q_OBJECT

public:

  RuleWindow();

private:

  Ui::RuleWindow m_ui;

};

#endif // __RULE_WINDOW_H__
