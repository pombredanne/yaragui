#ifndef __RULESET_H__
#define __RULESET_H__

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

  boost::property_tree::ptree serialize() const;

private:

  std::string m_file;

};

#endif // __RULESET_H__
