#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"

int main()
{
    CommonObjects common;
    DrawThread draw;
    auto draw_th = std::jthread([&] { draw(common); });
    DownloadThread down;
    auto down_th = std::jthread([&] { down(common); });

    while (!common.exit_flag)
    {
        if (!common.continents_ready)
        {
            down.FetchContinents(common);
        }
        else if (!common.cities_ready && !common.area.empty())
        {
            down.FetchCities(common, common.area);
        }
        else if (!common.data_ready && !common.area.empty() && !common.location.empty())
        {
            std::cout << "Setting URL with area: " << common.area << " and location: " << common.location << std::endl;
            down.SetUrl(common.area, common.location);
            down(common); // Trigger the download thread
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Add sleep to avoid busy waiting
    }

    std::cout << "running...\n";
    down_th.join();
    draw_th.join();
    return 0;
}
