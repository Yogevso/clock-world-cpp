#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <unordered_set>
#include "nlohmann/json.hpp"

// Define valid continents
extern const std::unordered_set<std::string> valid_continents;

struct CommonObjects
{
    std::atomic<bool> data_ready{ false };
    std::atomic<bool> exit_flag{ false };
    std::atomic<bool> continents_ready{ false };
    std::atomic<bool> cities_ready{ false };
    std::string time_data;
    std::string timezone;
    std::string utc_offset;
    std::string area;
    std::string location;
    std::vector<std::string> continents;
    std::vector<std::string> cities;
};
