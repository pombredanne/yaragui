#include "main_controller.h"
#include <boost/make_shared.hpp>

MainController::MainController(int argc, char* argv[], boost::asio::io_service& io) : m_io(io), m_haveRuleset(false)
{
  m_settings = boost::make_shared<Settings>();

  m_rm = boost::make_shared<RulesetManager>(boost::ref(io), m_settings);
  m_rm->onScanResult.connect(boost::bind(&MainController::handleScanResult, this, _1, _2));
  m_rm->onRulesUpdated.connect(boost::bind(&MainController::handleRulesUpdated, this));

  m_mainWindow = boost::make_shared<MainWindow>();
  m_mainWindow->onChangeTargets.connect(boost::bind(&MainController::handleChangeTargets, this, _1));
  m_mainWindow->onChangeRuleset.connect(boost::bind(&MainController::handleChangeRuleset, this, _1));
  m_mainWindow->onRequestRuleWindowOpen.connect(boost::bind(&MainController::handleRequestRuleWindowOpen, this));
  m_mainWindow->onRequestAboutWindowOpen.connect(boost::bind(&MainController::handleAboutWindowOpen, this));

  m_mainWindow->setRules(m_rm->getRules());
}

void MainController::handleChangeTargets(const std::vector<std::string>& files)
{
  m_targets = files;
  scan();
}

void MainController::handleChangeRuleset(RulesetView::Ref ruleset)
{
  m_ruleset = ruleset;
  m_haveRuleset = true;
  scan();
}

void MainController::handleScanResult(const std::string& target, ScannerRule::Ref rule)
{
  m_mainWindow->addScanResult(target, rule);
}

void MainController::handleRulesUpdated()
{
  m_mainWindow->setRules(m_rm->getRules());
}

void MainController::handleRequestRuleWindowOpen()
{
  if (m_ruleWindow && m_ruleWindow->isVisible()) {
    return;
  }

  m_ruleWindow = boost::make_shared<RuleWindow>();
  m_ruleWindow->onSaveRules.connect(boost::bind(&MainController::handleRuleWindowSave, this, _1));
  m_ruleWindow->setRules(m_rm->getRules());
}

void MainController::handleRuleWindowSave(const std::vector<RulesetView::Ref>& rules)
{
  m_rm->updateRules(rules);
}

void MainController::handleAboutWindowOpen()
{
  if (m_aboutWindow && m_aboutWindow->isVisible()) {
    return;
  }

  m_aboutWindow = boost::make_shared<AboutWindow>();
}

void MainController::scan()
{
  if (!m_targets.empty() && m_haveRuleset) {
    m_rm->scan(m_targets, m_ruleset);
  }
}
