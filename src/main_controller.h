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
  void compileCallback(Scanner::CompileResult::Ref result);
  boost::asio::io_service& m_io;
  boost::shared_ptr<Scanner> m_scanner;
  boost::shared_ptr<MainWindow> m_mainWindow;
};

#endif // __MAIN_CONTROLLER_H__
