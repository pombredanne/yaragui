#include "settings.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

Settings::~Settings()
{
  saveToDisk();
}

Settings::Settings()
{
  QDir dir = QCoreApplication::applicationDirPath();
  QString file = dir.absoluteFilePath("settings.json");
  m_file = file.toStdString();

  try {
    boost::property_tree::json_parser::read_json(m_file, m_tree);
  } catch (const std::exception& e) {
    /* settings is missing or corrupt. fresh start */
  }
}

bool Settings::saveToDisk()
{
  try {
    boost::property_tree::json_parser::write_json(m_file, m_tree);
  } catch (const std::exception& e) {
    return false;
  }
  return true;
}

std::vector<Ruleset::Ref> Settings::getRules() const
{
  std::vector<Ruleset::Ref> rules;
  boost::property_tree::ptree tree = m_tree.get_child("rules", boost::property_tree::ptree());
  BOOST_FOREACH(boost::property_tree::ptree::value_type ruleset, tree.get_child("")) {
    rules.push_back(boost::make_shared<Ruleset>(ruleset.second));
  }
  return rules;
}

void Settings::setRules(const std::vector<Ruleset::Ref>& rules)
{
  boost::property_tree::ptree tree;
  BOOST_FOREACH(Ruleset::Ref ruleset, rules) {
    tree.add_child("rule", ruleset->serialize());
  }
  m_tree.put_child("rules", tree);
  saveToDisk();
}
