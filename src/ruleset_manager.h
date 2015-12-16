#ifndef __RULESET_MANAGER_H__
#define __RULESET_MANAGER_H__

/* this is a high level interface to the scanner thread */
/* it deal with rule objects rather than rule file paths for the purpose of caching compiled rules */

#include "ruleset.h"
#include "scanner.h"
#include "settings.h"
#include <boost/asio.hpp>
#include <vector>

class RulesetManager
{
public:

  ~RulesetManager();
  RulesetManager(boost::asio::io_service& io, boost::shared_ptr<Settings> settings);

  void setTarget(const std::string& file);
  void setTargetDirectory(const std::string& path);
  void setRuleset(const Ruleset::Ref ruleset);

  std::vector<RulesetView::Ref> getRules() const;
  void createRule(const std::string& file);

private:

  boost::asio::io_service& m_io;

  boost::shared_ptr<Scanner> m_scanner;
  boost::shared_ptr<Settings> m_settings;

  std::vector<Ruleset::Ref> m_rules;

  std::list<std::string> m_activeTargets;
  std::list<Ruleset::Ref> m_activeRules;

};

#endif // __RULESET_MANAGER_H__
