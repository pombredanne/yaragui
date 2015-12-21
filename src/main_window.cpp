#include "main_window.h"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtCore/QMimeData>

MainWindow::MainWindow()
{
  m_ui.setupUi(this);
  setAcceptDrops(true); /* enable drag and drop */

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
  m_ui.rulePath->setText(tr("[All Rules]"));
}

void MainWindow::handleSelectRuleFromMenu(int rule)
{
  RulesetView::Ref view = m_rules[rule];
  m_ui.rulePath->setText(view->file().c_str());
  onChangeRuleset(view);
}

void MainWindow::handleTargetFileBrowse()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Target File", QString(), "All Files (*.*)");
  if (!file.isEmpty()) {
    m_ui.targetPath->setText(file);
    std::vector<std::string> targets;
    targets.push_back(file.toStdString());
    onChangeTargets(targets);
  }
}

void MainWindow::handleTargetDirectoryBrowse()
{
  QString dir = QFileDialog::getExistingDirectory(this, "Select Target Directory");
  if (!dir.isEmpty()) {
    m_ui.targetPath->setText(dir);
    std::vector<std::string> targets;
    targets.push_back(dir.toStdString());
    onChangeTargets(targets);
  }
}

void MainWindow::handleRuleFileBrowse()
{
  QString file = QFileDialog::getOpenFileName(this, "Select Rule File", QString(), "YARA Rules (*.yara *.yar)");
  if (!file.isEmpty()) {
    m_ui.rulePath->setText(file);
    onChangeRuleset(boost::make_shared<RulesetView>(file.toStdString()));
  }
}

void MainWindow::handleEditRulesMenu()
{
  onRequestRuleWindowOpen();
}

void MainWindow::handleAboutMenu()
{
  onRequestAboutWindowOpen();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  /* filter out non-files, like images dragged from the web browser */
  const QMimeData* mimeData = event->mimeData();
  if (!mimeData->hasUrls()) {
    event->ignore();
    return;
  }

  QList<QUrl> urls = mimeData->urls();
  for (int i = 0; i < urls.size(); ++i) {
    if (!urls[i].isLocalFile()) {
      event->ignore();
      return;
    }
  }

  event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* mimeData = event->mimeData();
  QList<QUrl> urls = mimeData->urls();

  /* if one file was dropped, check if it is a rule */
  if (urls.size() == 1) {
    QFileInfo fileInfo(urls[0].toLocalFile());
    QString file = QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    if (fileInfo.suffix() == "yar" || fileInfo.suffix() == "yara") {
      m_ui.rulePath->setText(file);
      onChangeRuleset(boost::make_shared<RulesetView>(file.toStdString()));
      event->acceptProposedAction();
      return; /* a rule was dropped */
    }
  }

  /* treat all other files as targets */
  std::vector<std::string> targets;
  for (int i = 0; i < urls.size(); ++i) {
    QFileInfo fileInfo(urls[i].toLocalFile());
    QString file = QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    targets.push_back(file.toStdString());
  }


  if (targets.size() == 1) {
    m_ui.targetPath->setText(targets[0].c_str());
  } else {
    m_ui.targetPath->setText(tr("[Multiple Targets]"));
  }

  onChangeTargets(targets);
  event->acceptProposedAction();
}
