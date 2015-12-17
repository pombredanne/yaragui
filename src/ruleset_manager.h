#ifndef __RULESET_MANAGER_H__
#define __RULESET_MANAGER_H__

/* this is a high level interface to the scanner thread */
/* it deal with rule objects rather than rule file paths for the purpose of caching compiled rules */

#include "ruleset.h"
#include "scanner.h"
#include "settings.h"
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <vector>
#include <list>

class RulesetManager
{
public:

  ~RulesetManager();
  RulesetManager(boost::asio::io_service& io, boost::shared_ptr<Settings> settings);

  boost::signals2::signal<void (const std::string& target, ScannerRule::Ref rule)> onScanResult;
  boost::signals2::signal<void (const std::string& error)> onScanComplete;

  void scan(const std::string& target, RulesetView::Ref view);

  std::vector<RulesetView::Ref> getRules() const;
  void createRule(const std::string& file);

private:

  void handleCompile(Scanner::CompileResult::Ref compileResult);
  void handleScanResult(ScannerRule::Ref rule);
  void handleScanComplete(const std::string& error);

  void compileNextRule();
  void scanWithCompiledRules();
  std::list<Ruleset::Ref> ruleToQueue(Ruleset::Ref rule);
  Ruleset::Ref viewToRule(RulesetView::Ref view);

  boost::asio::io_service& m_io;
  boost::shared_ptr<Scanner> m_scanner;
  boost::shared_ptr<Settings> m_settings;

  std::vector<Ruleset::Ref> m_rules;
  std::map<std::string, YR_RULES*> m_binaries;

  Ruleset::Ref m_activeRule;
  std::list<std::string> m_queueTargets;
  std::list<Ruleset::Ref> m_queueRules;

};

#endif // __RULESET_MANAGER_H__
