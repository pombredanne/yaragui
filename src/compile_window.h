#ifndef __COMPILE_WINDOW_H__
#define __COMPILE_WINDOW_H__

#include "ui_compile_window.h"
#include "ruleset_view.h"

class CompileWindow : public QMainWindow
{
  Q_OBJECT

public:

  CompileWindow(RulesetView::Ref view);

private:

  Ui::CompileWindow m_ui;

};

#endif // __COMPILE_WINDOW_H__
