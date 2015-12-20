#ifndef __RULE_WINDOW_H__
#define __RULE_WINDOW_H__

#include "ui_rule_window.h"
#include "ruleset_view.h"

class RuleWindow : public QMainWindow
{
  Q_OBJECT

public:

  RuleWindow();

  void setRules(const std::vector<RulesetView::Ref>& rules);

private slots:

private:

  Ui::RuleWindow m_ui;

  std::vector<RulesetView::Ref> m_rules;

  std::map<QTableWidgetItem*, RulesetView::Ref> m_names;

};

#endif // __RULE_WINDOW_H__
