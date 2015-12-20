#include "rule_window.h"

RuleWindow::RuleWindow()
{
  m_ui.setupUi(this);

  m_ui.table->setColumnCount(2);
  m_ui.table->verticalHeader()->hide();

  m_ui.table->setSelectionBehavior(QTableWidget::SelectRows);

  m_ui.table->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_ui.table->addAction(new QAction("Test Action", this));

  show();
}

void RuleWindow::setRules(const std::vector<RulesetView::Ref>& rules)
{
  m_rules = rules;
  m_ui.table->clear();
  m_ui.table->setRowCount(rules.size());
  for (size_t i = 0; i < rules.size(); ++i) {
    QTableWidgetItem* itemName = new QTableWidgetItem(rules[i]->name().c_str());
    m_ui.table->setItem(int(i), 0, itemName);
    m_names[itemName] = rules[i];

    QTableWidgetItem* itemFile = new QTableWidgetItem(rules[i]->file().c_str());
    m_ui.table->setItem(int(i), 1, itemFile);
  }
}
