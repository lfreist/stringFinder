// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_INPUTPARSER_H_
#define SRC_UTILS_INPUTPARSER_H_

#include <utility>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/classic.hpp>

#include <vector>
#include <string>

namespace x3 = boost::spirit::x3;

using std::string;


class Command {
 public:
  Command() = default;
  virtual ~Command() = default;
  virtual string type() {};
};

class SearchCommand : public Command {
  public:
  SearchCommand(string searchPattern, string targetFile, int limit) {
    _searchPattern = std::move(searchPattern);
    _targetFile = std::move(targetFile);
    _limit = limit;
  }
  ~SearchCommand() override = default;
  string type {"search"};
  string getSearchPattern() {
    return _searchPattern;
  }
  string getTargetFile() {
    return _targetFile;
  }
  [[nodiscard]] int getLimit() const {
    return _limit;
  }
 private:
  string _searchPattern;
  string _targetFile;
  int _limit;
  FRIEND_TEST(InputParserTest, searchQuery);
};

class LoadCommand : public Command {
 public:
  LoadCommand(string filepath, string alias) {
    _filepath = std::move(filepath);
    _alias = std::move(alias);
  }
  ~LoadCommand() override = default;
  string type {"load"};
  string getFilePath() {
    return _filepath;
  }
  string getAlias() {
    return _alias;
  }
 private:
  string _filepath;
  string _alias;
  FRIEND_TEST(InputParserTest, loadQuery);
};


template <typename Iterator>
auto parse(Iterator begin, Iterator end, bool &success) {
  string search;
  string load;
  string target;
  string alias;
  int limit = -1;

  auto toSearch = [&](auto &ctx) {search = x3::_attr(ctx);};
  auto toLoad   = [&](auto &ctx) {load   = x3::_attr(ctx);};
  auto toTarget = [&](auto &ctx) {target = x3::_attr(ctx);};
  auto toAlias  = [&](auto &ctx) {alias  = x3::_attr(ctx);};
  auto toLimit  = [&](auto &ctx) {limit  = x3::_attr(ctx);};

  auto keywords = x3::lit("SEARCH") | x3::lit("LOAD") | x3::lit("AS") | x3::lit("IN") | x3::lit("LIMIT");
  auto var      = *(x3::char_ - keywords);
  auto num      = x3::int_;

  auto search_clause = "SEARCH" >> var[toSearch] >> -("IN" >> var[toTarget]) >> -("LIMIT" >> num[toLimit]);
  auto load_clause   = "LOAD" >> var[toLoad] >> -("AS" >> var[toAlias]);
  auto query         = search_clause | load_clause;

  success = x3::phrase_parse(begin, end, query, x3::ascii::space);

  Command *cmd;

  if (!success) {
    cmd = new Command();
    return cmd;
  }
  if (search.empty()) {
    cmd = new LoadCommand(load, alias);
  } else {
    cmd = new SearchCommand(search, target, limit);
  }
  return cmd;
}


#endif  // SRC_UTILS_INPUTPARSER_H_