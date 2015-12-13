#include "main_controller.h"
#include <boost/make_shared.hpp>

MainController::MainController(boost::asio::io_service& io) : m_io(io)
{
  m_scanner = boost::make_shared<Scanner>(boost::ref(io));
  m_mainWindow = boost::make_shared<MainWindow>();
  m_mainWindow->onRequestScan.connect(boost::bind(&MainController::testScan, this, _1, _2));
}

void MainController::testScan(const std::string& target, const std::string& rules)
{
  m_target = target;
  m_rules = rules;

  std::cout << "Compiling " << rules << "..." << std::endl;
  m_scanner->rulesCompile(rules, "", boost::bind(&MainController::compileCallback, this, _1));
}

void MainController::compileCallback(Scanner::CompileResult::Ref result)
{
  std::cout << "Compiled: " << result->file << std::endl;
  if (result->error.empty()) {
    m_scanner->scanStart(result->rules, m_target, 0,
      boost::bind(&MainController::scanResultCallback, this, _1),
      boost::bind(&MainController::scanCompleteCallback, this, _1));
  }
}

void MainController::scanResultCallback(ScannerRule::Ref rule)
{
  if (rule) {
    std::cout << "Matched: " << rule->identifier << std::endl;
  }
}

void MainController::scanCompleteCallback(std::string error)
{
  if (!error.empty()) {
    std::cout << error << std::endl;
  }
  std::cout << "Scan complete." << std::endl;
}
