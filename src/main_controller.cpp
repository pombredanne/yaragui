#include "main_controller.h"
#include <boost/make_shared.hpp>

MainController::MainController(int argc, char* argv[], boost::asio::io_service& io) : m_io(io)
{
  m_settings = boost::make_shared<Settings>();
  m_rm = boost::make_shared<RulesetManager>(boost::ref(io), m_settings);
  m_mainWindow = boost::make_shared<MainWindow>();
  m_mainWindow->setRules(m_rm->getRules());
}
