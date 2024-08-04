#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <unordered_set>

// Define valid continents
const std::unordered_set<std::string> valid_continents = {
    "Africa", "America", "Antarctica", "Asia", "Atlantic", "Australia", "Europe", "Indian", "Pacific"
};

void DownloadThread::operator()(CommonObjects& common)
{
    if (_area.empty() || _location.empty()) {
        std::cerr << "Error: Area or location is empty." << std::endl;
        return;
    }
    std::string url = "/api/timezone/" + _area + "/" + _location;
    httplib::Client cli("http://worldtimeapi.org");
    auto res = cli.Get(url.c_str());

    if (res) {
        if (res->status == 200) {
            auto json_result = nlohmann::json::parse(res->body);
            common.time_data = json_result["datetime"].get<std::string>();
            common.timezone = json_result["timezone"].get<std::string>();
            common.utc_offset = json_result["utc_offset"].get<std::string>();
            common.data_ready = true;
        }
        else {
            std::cerr << "Error: Received response status code " << res->status << std::endl;
        }
    }
    else {
        std::cerr << "Error: Failed to connect to the API" << std::endl;
    }
}

void DownloadThread::SetUrl(std::string_view area, std::string_view location)
{
    _area = area;
    _location = location;
}

void DownloadThread::FetchContinents(CommonObjects& common)
{
    httplib::Client cli("http://worldtimeapi.org");
    auto res = cli.Get("/api/timezone");

    if (res) {
        if (res->status == 200) {
            auto json_result = nlohmann::json::parse(res->body);
            std::unordered_set<std::string> continents_set;
            for (const auto& item : json_result) {
                std::string continent = item.get<std::string>().substr(0, item.get<std::string>().find('/'));
                if (valid_continents.find(continent) != valid_continents.end()) {
                    continents_set.insert(continent);
                }
            }
            common.continents.assign(continents_set.begin(), continents_set.end());
            common.continents_ready = true;
        }
        else {
            std::cerr << "Error: Received response status code " << res->status << std::endl;
        }
    }
    else {
        std::cerr << "Error: Failed to connect to the API" << std::endl;
    }
}

void DownloadThread::FetchCities(CommonObjects& common, std::string_view continent)
{
    httplib::Client cli("http://worldtimeapi.org");
    std::string url = "/api/timezone/" + std::string(continent);
    auto res = cli.Get(url.c_str());

    if (res) {
        if (res->status == 200) {
            auto json_result = nlohmann::json::parse(res->body);
            common.cities.clear();
            for (const auto& item : json_result) {
                std::string city = item.get<std::string>().substr(item.get<std::string>().find('/') + 1);
                common.cities.push_back(city);
            }
            common.cities_ready = true;
        }
        else {
            std::cerr << "Error: Received response status code " << res->status << std::endl;
        }
    }
    else {
        std::cerr << "Error: Failed to connect to the API" << std::endl;
    }
}
