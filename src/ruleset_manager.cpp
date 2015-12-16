#include "ruleset_manager.h"
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <QtCore/QDir>

RulesetManager::~RulesetManager()
{
}

RulesetManager::RulesetManager(boost::asio::io_service& io, boost::shared_ptr<Settings> settings) : m_io(io), m_settings(settings)
{
  m_scanner = boost::make_shared<Scanner>(boost::ref(io));
  m_rules = m_settings->getRules();
}

void RulesetManager::setTarget(const std::string& file)
{
  m_activeTargets.clear();
  m_activeTargets.push_back(file);
}

void RulesetManager::setTargetDirectory(const std::string& path)
{
  m_activeTargets.clear();
  QDir dir(path.c_str());
  QStringList files = dir.entryList();
  for (size_t i = 0; i < files.size(); ++i) {
    m_activeTargets.push_back(files[i].toStdString());
  }
}

void RulesetManager::setRuleset(const Ruleset::Ref ruleset)
{
  m_activeRules.clear();
  if (ruleset) {
    m_activeRules.push_back(ruleset);
  } else { /* scan using all loaded rules */
    m_activeRules = std::list<Ruleset::Ref>(m_rules.begin(), m_rules.end());
  }
}

std::vector<RulesetView::Ref> RulesetManager::getRules() const
{
  /* we only provide external code with a "view" onto our ruleset */
  std::vector<RulesetView::Ref> views;
  BOOST_FOREACH(Ruleset::Ref ruleset, m_rules) {
    views.push_back(ruleset->view());
  }
  return views;
}

void RulesetManager::createRule(const std::string& file)
{
  Ruleset::Ref ruleset = boost::make_shared<Ruleset>(file);
  m_rules.push_back(ruleset);
  m_settings->setRules(m_rules);
  m_settings->saveToDisk();
}
