#ifndef __MAIN_CONTROLLER_H__
#define __MAIN_CONTROLLER_H__

#include "main_window.h"
#include "scanner.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

class MainController
{
public:
  MainController(boost::asio::io_service& io);
private:
  void testScan(const std::string& target, const std::string& rules);
  void compileCallback(Scanner::CompileResult::Ref result);
  void scanResultCallback(ScannerRule::Ref rule);
  void scanCompleteCallback(std::string error);
  boost::asio::io_service& m_io;
  boost::shared_ptr<Scanner> m_scanner;
  boost::shared_ptr<MainWindow> m_mainWindow;
  std::string m_target;
  std::string m_rules;
};

#endif // __MAIN_CONTROLLER_H__
