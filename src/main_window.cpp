#include "main_window.h"
#include <boost/foreach.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

MainWindow::MainWindow()
{
  m_ui.setupUi(this);

  QMenu* menu = new QMenu(this);
  m_ui.targetButton->setMenu(menu);

  menu->addAction("Scan &Directory");
  menu->addSeparator();
  menu->addAction("&About");

  m_ui.targetButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
  m_ui.ruleButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));

  show();
}

void MainWindow::setRules(const std::vector<Ruleset::Ref>& rules)
{
  QMenu* menu = new QMenu(this);
  m_ui.ruleButton->setMenu(menu);

  menu->addAction("&All Rules");
  menu->addSeparator();
  BOOST_FOREACH(const Ruleset::Ref ruleset, rules) {
    menu->addAction(ruleset->file().c_str());
  }

  if (!rules.empty()) {
    menu->addSeparator();
  }

  menu->addAction("&Configure");
}

void MainWindow::getUserInput()
{
  QString target = QFileDialog::getOpenFileName(this, "Select Target");
  QString rules = QFileDialog::getOpenFileName(this, "Select Rules", QString(), "Rules (*.yar)");
  onRequestScan(target.toStdString(), rules.toStdString());
}
