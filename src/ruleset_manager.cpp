#include "ruleset_manager.h"
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

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
  m_queueTargets.clear();
  m_queueTargets.push_back(target);

  m_activeRule = viewToRule(view);
  m_queueRules = ruleToQueue(m_activeRule); /* reload the queue for compiling */

  m_binaries.clear();
  compileNextRule();
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

Ruleset::Ref RulesetManager::createRule(const std::string& file)
{
  Ruleset::Ref ruleset = boost::make_shared<Ruleset>(file);
  m_rules.push_back(ruleset);
  return ruleset;
}

void RulesetManager::handleRuleCompile(Scanner::CompileResult::Ref compileResult)
{
  Ruleset::Ref ruleset = m_queueRules.front();
  m_binaries[ruleset->file()] = compileResult->rules;

  /* write the compiled rules to the cache */
  m_scanner->rulesSave(compileResult->rules, compiledRuleCache(ruleset->hash()), boost::bind(&RulesetManager::handleRuleSave, this, _1));
}

void RulesetManager::handleScanResult(ScannerRule::Ref rule)
{
  if (rule) {
    onScanResult(m_queueTargets.front(), rule);
  }
}

void RulesetManager::handleScanComplete(const std::string& error)
{
  m_queueRules.pop_front();
  if (m_queueRules.empty()) {
    onScanResult(m_queueTargets.front(), ScannerRule::Ref());
    m_queueTargets.pop_front();
    if (m_queueTargets.empty()) {
      freeBinaries(); /* cleanup before signaling completed */
      return;
    }
  }
  scanWithCompiledRules();
}

void RulesetManager::handleRuleHash(const std::string& hash)
{
  Ruleset::Ref ruleset = m_queueRules.front();
  if (ruleset->hash() != hash) {
    /* rule file has changed. will have to compile */
    QFile::remove(compiledRuleCache(ruleset->hash()).c_str());
    ruleset->setHash(hash);
    m_scanner->rulesCompile(ruleset->file(), "", boost::bind(&RulesetManager::handleRuleCompile, this, _1));
  } else {
    /* try to load from the cache */
    m_scanner->rulesLoad(compiledRuleCache(hash), boost::bind(&RulesetManager::handleRuleLoad, this, _1));
  }
}

void RulesetManager::handleRuleLoad(Scanner::LoadResult::Ref loadResult)
{
  Ruleset::Ref ruleset = m_queueRules.front();
  if (!loadResult->error.empty()) {
    /* failed to load the rules, will have to compile anyway */
    m_scanner->rulesCompile(ruleset->file(), "", boost::bind(&RulesetManager::handleRuleCompile, this, _1));
  } else {
    /* loaded from the cache */
    m_binaries[ruleset->file()] = loadResult->rules;
    m_queueRules.pop_front();
    compileNextRule();
  }
}

void RulesetManager::handleRuleSave(const std::string& error)
{
  /* cache updated */
  m_queueRules.pop_front();
  compileNextRule();
}

void RulesetManager::compileNextRule()
{
  if (m_queueRules.empty()) {
    scanWithCompiledRules();
    return;
  }

  Ruleset::Ref ruleset = m_queueRules.front();
  m_scanner->rulesHash(ruleset->file(), boost::bind(&RulesetManager::handleRuleHash, this, _1));
}

void RulesetManager::scanWithCompiledRules()
{
  /* before we begin, write any cache updates to the settings file */
  m_settings->setRules(m_rules);

  const std::string target = *m_queueTargets.begin();
  QFileInfo fileInfo(target.c_str());
  if (fileInfo.isDir()) {
    m_queueTargets.pop_front();
    QDir dir(target.c_str());
    QStringList files = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (int i = 0; i < files.size(); ++i) {
      int j = files.size() - i - 1;
      QString fullPath = dir.absoluteFilePath(files[j]);
      m_queueTargets.push_front(fullPath.toStdString());
    }
  }

  if (m_queueRules.empty()) {
    m_queueRules = ruleToQueue(m_activeRule); /* reload the queue for scanning */
  }

  YR_RULES* rules = m_binaries[m_queueRules.front()->file()];

  m_scanner->scanStart(rules, target, 0,
    boost::bind(&RulesetManager::handleScanResult, this, _1),
    boost::bind(&RulesetManager::handleScanComplete, this, _1));
}

void RulesetManager::freeBinaries()
{
  if (m_binaries.empty()) {
    onScanComplete(std::string());
  } else {
    YR_RULES* rules = m_binaries.begin()->second;
    m_binaries.erase(m_binaries.begin()->first);
    m_scanner->rulesDestroy(rules, boost::bind(&RulesetManager::freeBinaries, this));
  }
}

std::list<Ruleset::Ref> RulesetManager::ruleToQueue(Ruleset::Ref rule)
{
  std::list<Ruleset::Ref> rules;
  if (!rule) { /* scanning with all rules */
    rules = std::list<Ruleset::Ref>(m_rules.begin(), m_rules.end());
  } else { /* scanning with a single rule */
    rules.push_back(m_activeRule);
  }
  return rules;
}

Ruleset::Ref RulesetManager::viewToRule(RulesetView::Ref view)
{
  if (!view) { /* means all rules */
    return Ruleset::Ref();
  }
  /* the filename is the key. could use a map here */
  BOOST_FOREACH(Ruleset::Ref ruleset, m_rules) {
    if (ruleset->file() == view->file()) {
      return ruleset;
    }
  }
  /* a new rule */
  return createRule(view->file());
}

std::string RulesetManager::compiledRuleCache(const std::string& hash) const
{
  /* store cached compiled rules in the executable path */
  QDir dir = QCoreApplication::applicationDirPath();
  QString cacheDirName = "cache";
  QString cacheDir = cacheDirName + QDir::separator() + hash.c_str();
  QString file = dir.absoluteFilePath(cacheDir);
  return file.toStdString();
}
