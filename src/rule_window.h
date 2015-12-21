#ifndef __RULE_WINDOW_H__
#define __RULE_WINDOW_H__

#include "ui_rule_window.h"
#include "ruleset_view.h"
#include <boost/signals2.hpp>
#include <boost/optional.hpp>

class RuleWindow : public QMainWindow
{
  Q_OBJECT

public:

  RuleWindow();

  boost::signals2::signal<void (const std::vector<RulesetView::Ref>& rules)> onSaveRules;
  boost::signals2::signal<void (RulesetView::Ref view)> onCompileRule;

  void setRules(const std::vector<RulesetView::Ref>& rules);

private slots:

  void handleButtonClicked(QAbstractButton* button);
  void handleItemEdit(QTableWidgetItem* item);
  void handleSelectionChanged();

  void handleCompileClicked();
  void handleMoveUpClicked();
  void handleMoveDownClicked();
  void handleRemoveClicked();

private:

  void rulesToView(const std::vector<RulesetView::Ref>& rules);
  std::vector<RulesetView::Ref> selectedItems();
  boost::optional<int> selectedItemIndex();

  Ui::RuleWindow m_ui;

  std::vector<RulesetView::Ref> m_rules;
  std::map<QTableWidgetItem*, RulesetView::Ref> m_names;

  QAction* m_compileButton;
  QAction* m_moveUpButton;
  QAction* m_moveDownButton;
  QAction* m_removeButton;

};

#endif // __RULE_WINDOW_H__
