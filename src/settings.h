#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "ruleset.h"
#include <boost/property_tree/ptree.hpp>
#include <vector>

class Settings
{
public:

  ~Settings();
  Settings();

  bool saveToDisk();

  std::vector<Ruleset::Ref> getRules() const;
  void setRules(const std::vector<Ruleset::Ref>& rules);

private:

  boost::property_tree::ptree m_tree;

  std::string m_file;

};

#endif // __SETTINGS_H__
