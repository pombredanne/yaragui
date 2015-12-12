#include "asio_events.h"
#include <boost/make_shared.hpp>

AsioEvents::AsioEvents(boost::asio::io_service& io)
: m_io(io)
{
  m_timer = boost::make_shared<QTimer>();
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));
  m_timer->setInterval(0);
  m_timer->start();
}

void AsioEvents::tick()
{
  m_io.poll_one();
}
