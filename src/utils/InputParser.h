// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#ifndef SRC_UTILS_INPUTPARSER_H_
#define SRC_UTILS_INPUTPARSER_H_

#include <utility>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/classic.hpp>

#include <vector>
#include <string>

#include "../StringFinder.h"

namespace x3 = boost::spirit::x3;

using std::string;

// helper function to get the number of digits an integer consists of
int getNumDigits(ulong number) {
  int maxDigits = 1;
  while (number /= 10) {
    maxDigits++;
  }
  return maxDigits;
}

// helper function to print a result line in a pretty way
void printPrettyResultLine(const string& line, ulong num, int maxDigits) {
  std::cout << std::right << std::setw(maxDigits) << num << ". " << line << std::endl;
}


class Command {
 public:
  Command() = default;
  virtual void execute(StringFinder &sf) = 0;
};

class SearchCommand : public virtual Command {
 public:
  SearchCommand(string searchPattern, string targetFile, int limit, const vector<string>& withAttr = {}) {
    _searchPattern = std::move(searchPattern);
    _targetFile = std::move(targetFile);
    _performance = std::find(withAttr.begin(), withAttr.end(), "performance") != withAttr.end();
    _matchCase = std::find(withAttr.begin(), withAttr.end(), "matchCase") != withAttr.end();
    _limit = limit;
  }

  ~SearchCommand() = default;

  void execute(StringFinder &sf) override {
    if (_performance) {
      sf.measurePerformance(_searchPattern, _matchCase);
      return;
    }
    auto result = sf.find(_searchPattern, _matchCase);
    ulong numPrintResults = _limit < 0 ? result.size() : _limit;
    int maxDigits = getNumDigits(numPrintResults);
    ulong counter = 1;
    std::cout << "Results: " << _limit << std::endl;
    for (auto &line : result) {
      printPrettyResultLine(*line, counter, maxDigits);
      if (counter++ >= numPrintResults) {
        break;
      }
    }
  }

 private:
  string _searchPattern;
  string _targetFile;
  bool _performance;
  bool _matchCase;
  int _limit;
  FRIEND_TEST(InputParserTest, searchQuery);
};

class LoadCommand : public virtual Command {
 public:
  explicit LoadCommand(string filepath, string alias = "") {
    _filepath = std::move(filepath);
    _alias = alias.empty() ? _filepath : std::move(alias);
  }

  ~LoadCommand() = default;

  void execute(StringFinder &sf) override {
    sf.readFile(_filepath, _alias);
    // std::cout << "Reading " << sf.numLines(_alias) << " from '" << _filepath << "' as '" << _alias << "'" << std::endl;
  }

 private:
  string _filepath;
  string _alias;
  FRIEND_TEST(InputParserTest, loadQuery);
};

class ListCommand : public virtual Command {
 public:
  ListCommand() = default;
  ~ListCommand() = default;
  void execute(StringFinder &sf) override {
    // TODO
  }
};

class ResetCommand : public virtual Command {
 public:
  ResetCommand() = default;
  ~ResetCommand() = default;
  void execute(StringFinder &sf) override {
    sf.reset();
  }
};


struct SFCliGrammar {};

template<typename Iterator>
std::optional<Command*> parse(Iterator begin, Iterator end) {
  string searchPattern;
  string fileName;
  string alias;
  vector<string> withAttr;
  int limit = -1;

  auto toSearch = [&](auto &ctx) { searchPattern = x3::_attr(ctx); };
  auto toFileName = [&](auto &ctx) { fileName = x3::_attr(ctx); };
  auto toAlias = [&](auto &ctx) { alias = x3::_attr(ctx); };
  auto toLimit = [&](auto &ctx) { limit = x3::_attr(ctx); };
  auto toWithAttr = [&](auto &ctx) { withAttr = x3::_attr(ctx); };

  auto keywords = x3::lit("search") | x3::lit("load") | x3::lit("as") | x3::lit("in") | x3::lit("limit") |
      x3::lit("reset") | x3::lit("with");

  auto var = *(x3::char_ - keywords);
  auto num = x3::int_;

  auto search_clause = "search" >> var[toSearch] >> -("in" >> var[toFileName]) >> -("limit" >> num[toLimit])
      >> -("with" >> (var % ",")[toWithAttr]);
  auto load_clause = "load" >> var[toFileName] >> -("as" >> var[toAlias]);
  auto reset_clause = "reset";
  auto query = search_clause | load_clause | reset_clause;

  bool success = x3::phrase_parse(begin, end, query, x3::ascii::space);

  if (!success) {
    std::cout << "ParsingError: Failed parsing input..." << std::endl;
    return {};
  }
  if (!searchPattern.empty()) {
    std::cout << "search..." << limit << std::endl;
    return new SearchCommand(searchPattern, fileName, limit, withAttr);
  } else if (!fileName.empty()) {
    return new LoadCommand(fileName, alias);
  }
  return new ResetCommand();
}

#endif  // SRC_UTILS_INPUTPARSER_H_