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

void RulesetManager::scan(const std::string& target, RulesetView::Ref view)
{
  m_target = target;
  Ruleset::Ref ruleset = viewToRule(view);
  m_scanner->rulesCompile(ruleset->file(), "", boost::bind(&RulesetManager::handleCompile, this, _1));
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

void RulesetManager::handleCompile(Scanner::CompileResult::Ref compileResult)
{
  m_compileResult = compileResult;
  m_scanner->scanStart(m_compileResult->rules, m_target, 0,
    boost::bind(&RulesetManager::handleScanResult, this, _1),
    boost::bind(&RulesetManager::handleScanComplete, this, _1));
}

void RulesetManager::handleScanResult(ScannerRule::Ref rule)
{
  if (rule) {
    onScanResult(m_target, rule);
  }
}

void RulesetManager::handleScanComplete(const std::string& error)
{
  onScanComplete(error);
}

Ruleset::Ref RulesetManager::viewToRule(RulesetView::Ref view)
{
  /* the filename is the key. could use a map here */
  BOOST_FOREACH(Ruleset::Ref ruleset, m_rules) {
    if (ruleset->file() == view->file()) {
      return ruleset;
    }
  }
  return boost::make_shared<Ruleset>(view->file());
}
