#ifndef __SCANNER_RULE_H__
#define __SCANNER_RULE_H__

#include <boost/shared_ptr.hpp>
#include <yara/types.h>
#include <vector>

struct ScannerRule
{
  typedef boost::shared_ptr<ScannerRule> Ref;

  ScannerRule(YR_RULE* rule);

  struct Match
  {
    typedef boost::shared_ptr<Match> Ref;
    std::vector<uint8_t> data;
    uint64_t base;
    uint64_t offset;
  };

  struct String
  {
    typedef boost::shared_ptr<String> Ref;
    std::string identifier;
    std::vector<uint8_t> value;
    std::vector<Match::Ref> matches;
  };

  struct Meta
  {
    typedef boost::shared_ptr<Meta> Ref;
    std::string identifier;
    std::string value;
  };

  std::string identifier;
  std::string ns; /* namespace */
  std::vector<std::string> tags;
  std::vector<String::Ref> strings;
  std::vector<Meta::Ref> metas;
};

#endif // __SCANNER_RULE_H__
