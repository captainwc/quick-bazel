#include <regex>
#include <vector>

#include "absl/strings/str_split.h"
#include "benchmark/benchmark.h"
#include "downloader.h"
#include "re2/re2.h"

const char* PATTERN = R"(\s*(?:const|var)\s+\w+\s*=\s+\w+\.(\w+)\(.*?\);?)";
auto        data    = FetchUrl("https://kaikaixixi.xyz").value_or("");

static void BM_RE2(benchmark::State& state) {
    for (auto _ : state) {
        RE2                      re(PATTERN);
        std::vector<std::string> funcs;
        re2::StringPiece         input(data);
        std::string              funcname;
        while (re2::RE2::FindAndConsume(&input, re, &funcname)) {
            funcs.push_back(funcname);
        };
    }
}

static void BM_STDREGEX(benchmark::State& state) {
    for (auto _ : state) {
        std::regex               re(PATTERN);
        std::vector<std::string> funcs;
        auto                     begin = std::sregex_iterator(data.begin(), data.end(), re);
        auto                     end   = std::sregex_iterator();
        for (auto i = begin; i != end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                funcs.emplace_back(match[1]);
            }
        }
    }
}

static void BM_RE2_LINES(benchmark::State& state) {
    auto lines = absl::StrSplit(data, "\n");
    for (auto _ : state) {
        re2::RE2                 re(PATTERN);
        std::vector<std::string> funcs;
        std::string              funcname;
        for (auto& line : lines) {
            if (re2::RE2::FullMatch(line, re, &funcname)) {
                funcs.push_back(funcname);
            }
        }
    }
}

static void BM_STDREGEX_LINES(benchmark::State& state) {
    auto lines = absl::StrSplit(data, "\n");
    for (auto _ : state) {
        std::regex               re(PATTERN);
        std::vector<std::string> funcs;
        std::cmatch              match;
        for (auto line : lines) {
            if (std::regex_match(line.data(), match, re)) {
                funcs.push_back(match[1]);
            }
        }
    }
}

BENCHMARK(BM_RE2);
BENCHMARK(BM_RE2_LINES);
BENCHMARK(BM_STDREGEX);
BENCHMARK(BM_STDREGEX_LINES);

BENCHMARK_MAIN();
