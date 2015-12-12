#include "main_controller.h"
#include <boost/make_shared.hpp>

MainController::MainController(boost::asio::io_service& io) : m_io(io)
{
  m_scanner = boost::make_shared<Scanner>(boost::ref(io));
  m_mainWindow = boost::make_shared<MainWindow>();

  m_scanner->compile("test.yara", "namespace", boost::bind(&MainController::compileCallback, this, _1));
}

void MainController::compileCallback(Scanner::CompileResult::Ref result)
{
  std::cout << "Compiled: " << result->file << std::endl;
}
