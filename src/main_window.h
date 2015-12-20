#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "ui_main_window.h"
#include "ruleset_view.h"
#include "scanner_rule.h"
#include <boost/signals2.hpp>
#include <QtCore/QSignalMapper>
#include <QtWidgets/QFileIconProvider>

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  MainWindow();

  boost::signals2::signal<void (const std::string& file)> onChangeTarget;
  boost::signals2::signal<void (RulesetView::Ref ruleset)> onChangeRuleset;
  boost::signals2::signal<void ()> onRequestRuleWindowOpen;
  boost::signals2::signal<void ()> onRequestAboutWindowOpen;

  void setRules(const std::vector<RulesetView::Ref>& rules);
  void addScanResult(const std::string& target, ScannerRule::Ref rule);

public slots:

  void handleSelectRuleAllFromMenu();
  void handleSelectRuleFromMenu(int rule);
  void handleTargetFileBrowse();
  void handleTargetDirectoryBrowse();
  void handleRuleFileBrowse();
  void handleEditRulesMenu();
  void handleAboutMenu();

private:

  Ui::MainWindow m_ui;
  QSignalMapper* m_signalMapper;

  QFileIconProvider m_iconProvider;

  std::vector<RulesetView::Ref> m_rules;

  std::map<std::string, QTreeWidgetItem*> m_treeItems;

};

#endif // __MAIN_WINDOW_H__
