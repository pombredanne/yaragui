#ifndef __ASIO_EVENTS_H__
#define __ASIO_EVENTS_H__

/* allow Qt and boost::asio to work together */

#include <boost/asio.hpp>
#include <QtCore/QTimer>

class AsioEvents : public QObject
{
  Q_OBJECT

public:

  AsioEvents(boost::asio::io_service& io);

private slots:

  void timeout();

private:

  boost::asio::io_service& m_io;
  boost::shared_ptr<QTimer> m_timer;

};

#endif // __ASIO_EVENTS_H__
