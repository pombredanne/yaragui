#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include "ruleset_view.h"
#include "ui_main_window.h"
#include <boost/signals2.hpp>
#include <QSignalMapper>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow();

  boost::signals2::signal<void (const std::string& file)> onChangeTarget;
  boost::signals2::signal<void (RulesetView::Ref ruleset)> onChangeRuleset;

  void setRules(const std::vector<RulesetView::Ref>& rules);

public slots:

  void handleSelectRuleAllFromMenu();
  void handleSelectRuleFromMenu(int rule);
  void handleTargetFileBrowse();
  void handleTargetDirectoryBrowse();
  void handleRuleFileBrowse();

private:

  Ui::MainWindow m_ui;
  QSignalMapper* m_signalMapper;

  std::vector<RulesetView::Ref> m_rules;

};

#endif // __MAIN_WINDOW_H__
