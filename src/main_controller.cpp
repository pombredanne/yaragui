#include "main_controller.h"
#include <boost/make_shared.hpp>

MainController::MainController(int argc, char* argv[], boost::asio::io_service& io) : m_io(io)
{
  m_settings = boost::make_shared<Settings>();

  m_rm = boost::make_shared<RulesetManager>(boost::ref(io), m_settings);
  m_rm->onScanResult.connect(boost::bind(&MainController::handleScanResult, this, _1, _2));

  m_mainWindow = boost::make_shared<MainWindow>();
  m_mainWindow->onChangeTarget.connect(boost::bind(&MainController::handleChangeTarget, this, _1));
  m_mainWindow->onChangeRuleset.connect(boost::bind(&MainController::handleChangeRuleset, this, _1));

  m_mainWindow->setRules(m_rm->getRules());
}

void MainController::handleChangeTarget(const std::string& file)
{
  m_target = file;
  scan();
}

void MainController::handleChangeRuleset(RulesetView::Ref ruleset)
{
  m_ruleset = ruleset;
  scan();
}

void MainController::handleScanResult(const std::string& target, ScannerRule::Ref rule)
{
  m_mainWindow->addScanResult(target, rule);
}

void MainController::scan()
{
  if (!m_target.empty() && m_ruleset) {
    m_rm->scan(m_target, m_ruleset);
  }
}
