#pragma once

#include <cstdint>
#include <vector>

enum class FilterType { None, Median, Min, Max };
enum class HistChannel { Colors, Luminance, Red, Green, Blue };

struct Stats {
  int min{0};
  int max{255};
  double avg{0.0};
};

struct HistData {
  std::vector<uint32_t> r{std::vector<uint32_t>(256, 0)};
  std::vector<uint32_t> g{std::vector<uint32_t>(256, 0)};
  std::vector<uint32_t> b{std::vector<uint32_t>(256, 0)};
  std::vector<uint32_t> gray{std::vector<uint32_t>(256, 0)};
  Stats stats;
};