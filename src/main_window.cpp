#include "main_window.h"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

MainWindow::MainWindow()
{
  m_ui.setupUi(this);

  QMenu* menu = new QMenu(this);
  m_ui.targetButton->setMenu(menu);

  QAction* scanDirectory = menu->addAction("Scan &Directory");
  connect(scanDirectory, SIGNAL(triggered()), this, SLOT(handleTargetDirectoryBrowse()));

  menu->addSeparator();
  QAction* about = menu->addAction("&About");
  connect(about, SIGNAL(triggered()), this, SLOT(handleAboutMenu()));

  m_ui.targetButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
  m_ui.ruleButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));

  connect(m_ui.targetButton, SIGNAL(released()), this, SLOT(handleTargetFileBrowse()));
  connect(m_ui.ruleButton, SIGNAL(released()), this, SLOT(handleRuleFileBrowse()));

  m_ui.tree->setColumnCount(2);
  m_ui.tree->header()->hide();
  m_ui.tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_ui.tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  m_ui.tree->header()->setStretchLastSection(false);

  show();
}

void MainWindow::setRules(const std::vector<RulesetView::Ref>& rules)
{
  m_rules = rules;

  QMenu* menu = new QMenu(this);
  m_ui.ruleButton->setMenu(menu);

  QAction* allRules = menu->addAction("&All Rules");
  connect(allRules, SIGNAL(triggered()), this, SLOT(handleSelectRuleAllFromMenu()));

  if (rules.empty()) {
    allRules->setEnabled(false);
  }

  menu->addSeparator();

  m_signalMapper = new QSignalMapper(this);
  connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(handleSelectRuleFromMenu(int)));

  for (size_t i = 0; i < rules.size(); ++i) {
    QAction* action = 0;
    if (rules[i]->hasName()) {
      action = menu->addAction(rules[i]->name().c_str());
    } else {
      action = menu->addAction(rules[i]->file().c_str());
    }
    connect(action, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(action, int(i));
  }

  if (!rules.empty()) {
    menu->addSeparator();
  }

  QAction* configure = menu->addAction("&Configure");
  connect(configure, SIGNAL(triggered()), this, SLOT(handleEditRulesMenu()));
}

void MainWindow::addScanResult(const std::string& target, ScannerRule::Ref rule)
{
  QTreeWidgetItem* root = 0;
  if (m_treeItems.find(target) != m_treeItems.end()) {
    root = m_treeItems[target];
  } else {
    root = new QTreeWidgetItem(m_ui.tree);
    m_treeItems[target] = root;
    m_ui.tree->insertTopLevelItem(0, root);
    root->setText(0, target.c_str());
    root->setText(1, tr("No matches"));

    /* set the file icon */
    QFileInfo fileInfo(target.c_str());
    root->setIcon(0, m_iconProvider.icon(fileInfo));
  }

  if (!rule) {
    return;
  }

  QTreeWidgetItem* item = new QTreeWidgetItem(root);
  item->setText(0, rule->identifier.c_str());

  if (root->childCount() == 1) {
    root->setText(1, tr("1 match"));
  } else {
    root->setText(1, tr("%1 matches").arg(root->childCount()));
  }

  if (root->childCount() == 1) {
    root->setExpanded(true); /* only expand when we add the first item */
  }
}

void MainWindow::handleSelectRuleAllFromMenu()
{
  /* null pointer means scan with every rule */
  onChangeRuleset(RulesetView::Ref());
}

void MainWindow::handleSelectRuleFromMenu(int rule)
{
  onChangeRuleset(m_rules[rule]);
}

void MainWindow::handleTargetFileBrowse()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Target File", QString(), "All Files (*.*)");
  m_ui.targetPath->setText(file);
  onChangeTarget(file.toStdString());
}

void MainWindow::handleTargetDirectoryBrowse()
{
  QString dir = QFileDialog::getExistingDirectory(this, "Select Target Directory");
  m_ui.targetPath->setText(dir);
  onChangeTarget(dir.toStdString());
}

void MainWindow::handleRuleFileBrowse()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Rule File", QString(), "YARA Rules (*.yara *.yar)");
  m_ui.rulePath->setText(file);
  onChangeRuleset(boost::make_shared<RulesetView>(file.toStdString()));
}

void MainWindow::handleEditRulesMenu()
{
  onRequestRuleWindowOpen();
}

void MainWindow::handleAboutMenu()
{
  onRequestAboutWindowOpen();
}
