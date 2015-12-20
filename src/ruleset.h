#ifndef __RULESET_H__
#define __RULESET_H__

#include "ruleset_view.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

class Ruleset
{
public:

  typedef boost::shared_ptr<Ruleset> Ref;

  ~Ruleset();
  Ruleset(const boost::property_tree::ptree& properties);
  Ruleset(const std::string& file);

  std::string file() const;

  std::string name() const;
  void setName(const std::string& name);

  std::string hash() const;
  void setHash(const std::string& hash);

  std::string compilerMessages() const;
  void setCompilerMessages(const std::string& compilerMessages);

  RulesetView::Ref view() const;
  boost::property_tree::ptree serialize() const;

private:

  std::string m_file;
  std::string m_name;
  std::string m_hash;
  std::string m_compilerMessages;

};

#endif // __RULESET_H__
