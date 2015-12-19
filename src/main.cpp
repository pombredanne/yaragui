#include "main_controller.h"
#include "asio_events.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
  boost::asio::io_service io;
  QApplication app(argc, argv);
  AsioEvents asio(io);
  MainController mainController(argc, argv, io);
  return app.exec();
}
