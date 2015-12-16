#ifndef __RULESET_VIEW_H__
#define __RULESET_VIEW_H__

#include <boost/shared_ptr.hpp>
#include <string>

class RulesetView
{
public:

  typedef boost::shared_ptr<RulesetView> Ref;

  RulesetView(const std::string& file);
  RulesetView(const std::string& file, const std::string& name);

  std::string file() const;

  bool hasName() const;
  std::string name() const;
  void setName(const std::string& name);

private:

  std::string m_file;
  std::string m_name;

};

#endif // __RULESET_VIEW_H__
