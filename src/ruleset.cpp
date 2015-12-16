#include "ruleset.h"
#include <boost/make_shared.hpp>

Ruleset::~Ruleset()
{
}

Ruleset::Ruleset(const boost::property_tree::ptree& properties)
{
  m_file = properties.get<std::string>("file", "");
  m_name = properties.get<std::string>("name", "");
}

Ruleset::Ruleset(const std::string& file) : m_file(file)
{
}

std::string Ruleset::file() const
{
  return m_file;
}

std::string Ruleset::name() const
{
  return m_name;
}

void Ruleset::setName(const std::string& name)
{
  m_name = name;
}

boost::shared_ptr<RulesetView> Ruleset::view() const
{
  return boost::make_shared<RulesetView>(m_file, m_name);
}

boost::property_tree::ptree Ruleset::serialize() const
{
  boost::property_tree::ptree properties;
  properties.put("file", m_file);
  if (!m_name.empty()) {
    properties.put("name", m_name);
  }
  return properties;
}
