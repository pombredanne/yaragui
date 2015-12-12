#include "asio_events.h"
#include <boost/make_shared.hpp>
#include <iostream>

AsioEvents::AsioEvents(boost::asio::io_service& io) : m_io(io)
{
  m_timer = boost::make_shared<QTimer>();
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(timeout()));
  m_timer->setInterval(0);
  m_timer->start();
}

void AsioEvents::timeout()
{
  boost::system::error_code error;
  m_io.poll_one(error);
  m_io.reset();
}
