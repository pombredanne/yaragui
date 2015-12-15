#include "ruleset.h"

Ruleset::~Ruleset()
{
}

Ruleset::Ruleset(const boost::property_tree::ptree& properties)
{
  m_file = properties.get<std::string>("file", "");
}

Ruleset::Ruleset(const std::string& file) : m_file(file)
{
}

boost::property_tree::ptree Ruleset::serialize() const
{
  boost::property_tree::ptree properties;
  properties.put("file", m_file);
  return properties;
}
