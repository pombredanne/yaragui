#include "main_controller.h"
#include "asio_events.h"
#include <QApplication>

int main(int argc, char* argv[])
{
  boost::asio::io_service io;
  QApplication app(argc, argv);
  AsioEvents asio(io);
  MainController mainController(io);
  return app.exec();
}
