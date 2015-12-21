#include "compile_window.h"

CompileWindow::CompileWindow(RulesetView::Ref view)
{
  m_ui.setupUi(this);

  m_ui.rulePath->setText(view->file().c_str());

  show();
}
