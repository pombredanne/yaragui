#include "main_controller.h"
#include <boost/make_shared.hpp>
#include <iostream>

MainController::MainController(int argc, char* argv[], boost::asio::io_service& io) : m_io(io)
{
  m_settings = boost::make_shared<Settings>();
  m_rm = boost::make_shared<RulesetManager>(boost::ref(io), m_settings);

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

void MainController::scan()
{
  if (!m_target.empty() && m_ruleset) {
    std::cout << m_target << " ---- " << m_ruleset->file() << std::endl;
  }
}
