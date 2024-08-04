#pragma once
#include "CommonObject.h"
#include <string_view>

class DownloadThread
{
public:
    void operator()(CommonObjects& common);
    void SetUrl(std::string_view area, std::string_view location);
    void FetchContinents(CommonObjects& common);
    void FetchCities(CommonObjects& common, std::string_view continent);
private:
    std::string _area;
    std::string _location;
};
