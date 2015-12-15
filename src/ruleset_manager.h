#ifndef __RULESET_MANAGER_H__
#define __RULESET_MANAGER_H__

/* this is a high level interface to the scanner thread */
/* it deal with rule objects rather than rule file paths for the purpose of cacheing compiled rules */

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

private:

  boost::asio::io_service& m_io;

  std::vector<Ruleset::Ref> m_rules;

  boost::shared_ptr<Scanner> m_scanner;

  boost::shared_ptr<Settings> m_settings;

};

#endif // __RULESET_MANAGER_H__
