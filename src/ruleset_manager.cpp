#include "ruleset_manager.h"
#include <boost/make_shared.hpp>

RulesetManager::~RulesetManager()
{
}

RulesetManager::RulesetManager(boost::asio::io_service& io, boost::shared_ptr<Settings> settings) : m_io(io), m_settings(settings)
{
  m_scanner = boost::make_shared<Scanner>(boost::ref(io));
  m_rules = m_settings->getRules();
}
