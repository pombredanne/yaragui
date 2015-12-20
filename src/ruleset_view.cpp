#include "ruleset_view.h"

RulesetView::RulesetView(const std::string& file) : m_file(file)
{
}

RulesetView::RulesetView(const std::string& file, const std::string& name, bool compiled) : m_file(file), m_name(name), m_compiled(compiled)
{
}

std::string RulesetView::file() const
{
  return m_file;
}

bool RulesetView::isCompiled() const
{
  return m_compiled;
}

bool RulesetView::hasName() const
{
  return !m_name.empty();
}

std::string RulesetView::name() const
{
  return m_name;
}

void RulesetView::setName(const std::string& name)
{
  m_name = name;
}
