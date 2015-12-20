#include "rule_window.h"

RuleWindow::RuleWindow()
{
  m_ui.setupUi(this);

  connect(m_ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClicked(QAbstractButton*)));

  m_ui.table->setColumnCount(3);
  m_ui.table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui.table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_ui.table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

  m_ui.table->verticalHeader()->hide();

  m_ui.table->setSelectionBehavior(QTableWidget::SelectRows);

  m_ui.table->setContextMenuPolicy(Qt::ActionsContextMenu);
  m_ui.table->addAction(new QAction("Compile", this));
  m_ui.table->addAction(new QAction("Move Up", this));
  m_ui.table->addAction(new QAction("Move Down", this));
  m_ui.table->addAction(new QAction("Remove", this));

  connect(m_ui.table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleItemEdit(QTableWidgetItem*)));

  show();
}

void RuleWindow::setRules(const std::vector<RulesetView::Ref>& rules)
{
  m_rules = rules;
  m_ui.table->clear();
  m_ui.table->setRowCount(rules.size());

  QStringList headerLabels;
  headerLabels << "Name" << "File" << "Compiled";
  m_ui.table->setHorizontalHeaderLabels(headerLabels);

  for (size_t i = 0; i < rules.size(); ++i) {
    QTableWidgetItem* itemName = new QTableWidgetItem(rules[i]->name().c_str());
    m_ui.table->setItem(int(i), 0, itemName);
    m_names[itemName] = rules[i];

    QTableWidgetItem* itemFile = new QTableWidgetItem(rules[i]->file().c_str());
    itemFile->setFlags(itemFile->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(int(i), 1, itemFile);

    QString compiledString = rules[i]->isCompiled() ? tr("Yes") : tr("No");
    QTableWidgetItem* itemCompiled = new QTableWidgetItem(compiledString);
    itemCompiled->setFlags(itemCompiled->flags() & ~Qt::ItemIsEditable);
    m_ui.table->setItem(int(i), 2, itemCompiled);
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