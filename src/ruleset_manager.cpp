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

const std::vector<Ruleset::Ref>& RulesetManager::getRules() const
{
  return m_rules;
}

const Ruleset::Ref RulesetManager::createRule(const std::string& file)
{
  Ruleset::Ref ruleset = boost::make_shared<Ruleset>(file);
  m_rules.push_back(ruleset);
  m_settings->setRules(m_rules);
  m_settings->saveToDisk();
  return ruleset;
}
