#include "rule_window.h"

RuleWindow::RuleWindow()
{
  m_ui.setupUi(this);

  connect(m_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClicked(QAbstractButton*)));

  m_ui.table->setColumnCount(2);
  m_ui.table->verticalHeader()->hide();

  m_ui.table->setSelectionBehavior(QTableWidget::SelectRows);

  m_ui.table->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_ui.table->addAction(new QAction("Test Action", this));

  connect(m_ui.table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));

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
    itemFile->setFlags(itemFile->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(int(i), 1, itemFile);
  }
}

void RuleWindow::handleButtonClicked(QAbstractButton* button)
{
  QDialogButtonBox::StandardButton choice = m_ui.buttonBox->standardButton(button);
  switch (choice) {
  case QDialogButtonBox::Ok:
    onSaveRules(m_rules);
    close();
    break;
  case QDialogButtonBox::Apply:
    onSaveRules(m_rules);
    break;
  case QDialogButtonBox::Cancel:
    close();
    break;
  default:
    break;
  }
}

void RuleWindow::handleItemEdit(QTableWidgetItem* item)
{
  if (m_names.find(item) == m_names.end()) {
    return; /* not a name cell */
  }
  m_names[item]->setName(item->text().toStdString());
}