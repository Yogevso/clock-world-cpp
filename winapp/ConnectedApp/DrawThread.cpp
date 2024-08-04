#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif

// Function to parse the initial time fetched from the API and adjust it by subtracting one hour
std::tm parse_time(const std::string& datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    tm.tm_hour -= 1;  // Subtract one hour
    std::mktime(&tm); // Normalize the time structure after subtraction
    return tm;
}

// Function to draw the digital clock
void DrawDigitalClock(CommonObjects& common, float size) {
    if (common.data_ready) {
        // Calculate current time based on initial fetched time and offset
        static auto initial_time = std::chrono::system_clock::now();
        auto current_time = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - initial_time).count();

        auto tm = parse_time(common.time_data);
        tm.tm_sec += static_cast<int>(elapsed); // Explicitly cast to int

        std::mktime(&tm); // Normalize the time structure

        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Define clock rectangle dimensions
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        float vertical_offset = ImGui::GetFrameHeightWithSpacing() * 2; // Ensure it's below the slider
        float rect_height = window_size.y - vertical_offset - 10.0f; // Adjust height to fill the area
        float rect_width = window_size.x - 20.0f; // Adjust width to fill the area
        ImVec2 rect_min = ImVec2(window_pos.x + (window_size.x - rect_width) * 0.5f, window_pos.y + vertical_offset);
        ImVec2 rect_max = ImVec2(rect_min.x + rect_width, rect_min.y + rect_height);

        // Draw clock background
        draw_list->AddRectFilled(rect_min, rect_max, IM_COL32(50, 50, 50, 255), 10.0f);

        // Draw clock border
        draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 255), 10.0f, 0, 3.0f);

        // Calculate text size and position
        ImVec2 text_size = ImGui::CalcTextSize(buffer);
        float text_scale = size * 0.8f;
        ImVec2 text_pos = ImVec2((rect_min.x + rect_max.x - text_size.x * text_scale) * 0.5f, (rect_min.y + rect_max.y - text_size.y * text_scale) * 0.5f);
        draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize() * text_scale, text_pos, IM_COL32(255, 255, 255, 255), buffer);
    }
    else {
        ImGui::Text("Fetching data...");
    }
}

// Function to draw the analog clock
void DrawAnalogClock(CommonObjects& common, float radius) {
    if (common.data_ready) {
        // Calculate current time based on initial fetched time and offset
        static auto initial_time = std::chrono::system_clock::now();
        auto current_time = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - initial_time).count();

        auto tm = parse_time(common.time_data);
        tm.tm_sec += static_cast<int>(elapsed); // Explicitly cast to int

        std::mktime(&tm); // Normalize the time structure

        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        float vertical_offset = ImGui::GetFrameHeightWithSpacing() * 2; // Ensure it's below the slider
        ImVec2 center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + vertical_offset + radius);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Ensure minimum radius is 60
        if (radius < 60.0f) radius = 60.0f;

        // Adjust clock radius to fit within bounds
        if (center.x + radius > window_pos.x + window_size.x) {
            radius = (window_size.x - center.x + window_pos.x) / 2.0f;
        }
        if (center.y + radius > window_pos.y + window_size.y) {
            radius = (window_size.y - center.y + window_pos.y) / 2.0f;
        }

        // Draw clock face background
        draw_list->AddCircleFilled(center, radius + 5, IM_COL32(50, 50, 50, 255));

        // Draw clock face
        draw_list->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 60, 2.0f);

        // Draw clock numbers
        for (int i = 1; i <= 12; ++i) {
            float angle = (float)i * IM_PI / 6.0f - IM_PI / 2.0f;
            float x = center.x + cos(angle) * (radius - 20);
            float y = center.y + sin(angle) * (radius - 20);
            char num[3];
            snprintf(num, sizeof(num), "%d", i);
            ImVec2 text_size = ImGui::CalcTextSize(num);
            draw_list->AddText(ImVec2(x - text_size.x / 2, y - text_size.y / 2), IM_COL32(255, 255, 255, 255), num);
        }

        // Draw clock hands
        float second_angle = (float)tm.tm_sec * IM_PI / 30.0f - IM_PI / 2.0f;
        float minute_angle = ((float)tm.tm_min + (float)tm.tm_sec / 60.0f) * IM_PI / 30.0f - IM_PI / 2.0f;
        float hour_angle = ((float)tm.tm_hour + (float)tm.tm_min / 60.0f) * IM_PI / 6.0f - IM_PI / 2.0f;

        ImVec2 second_hand = ImVec2(center.x + cos(second_angle) * (radius - 10),
            center.y + sin(second_angle) * (radius - 10));
        ImVec2 minute_hand = ImVec2(center.x + cos(minute_angle) * (radius - 20),
            center.y + sin(minute_angle) * (radius - 20));
        ImVec2 hour_hand = ImVec2(center.x + cos(hour_angle) * (radius - 30),
            center.y + sin(hour_angle) * (radius - 30));

        draw_list->AddLine(center, second_hand, IM_COL32(255, 0, 0, 255), 1.0f);
        draw_list->AddLine(center, minute_hand, IM_COL32(255, 255, 255, 255), 3.0f);
        draw_list->AddLine(center, hour_hand, IM_COL32(255, 255, 255, 255), 5.0f);

        // Draw center circle
        draw_list->AddCircleFilled(center, 5, IM_COL32(255, 255, 255, 255));
    }
    else {
        ImGui::Text("Fetching data...");
    }
}

// Main ImGui rendering function
void DrawAppWindow(void* common_ptr)
{
    auto common = static_cast<CommonObjects*>(common_ptr);

    ImGui::Begin("What's the time?");

    static bool use_manual_input = false;
    static bool use_analog_clock = false;
    static float clock_size = 5.0f; // Internal size value (6 to 10)
    static float display_size = 3.0f; // Display size value (1 to 5)
    static std::string error_message;

    // Set button colors to RGB(50, 50, 50)
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255)); // Dark gray
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255)); // Slightly lighter gray when hovered
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255)); // Even lighter gray when active
    if (ImGui::Button(use_manual_input ? "Use Dropdowns" : "Use Manual Input")) {
        use_manual_input = !use_manual_input;
        error_message.clear(); // Clear error message when switching input modes
    }
    ImGui::PopStyleColor(3); // Reset button colors

    if (use_manual_input) {
        // Manual Input Fields
        ImGui::Separator();
        ImGui::Text("Manually input area and location:");

        // Set input field background colors to RGB(50, 50, 50)
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 50, 255)); // Dark gray
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(70, 70, 70, 255)); // Slightly lighter gray when hovered
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(90, 90, 90, 255)); // Even lighter gray when active
        static char manual_area[100] = "";
        static char manual_location[100] = "";
        ImGui::InputText("Manual Area", manual_area, sizeof(manual_area));
        ImGui::InputText("Manual Location", manual_location, sizeof(manual_location));
        ImGui::PopStyleColor(3); // Reset input field colors

        // Set button color for "Fetch Time Manually"
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));
        if (ImGui::Button("Fetch Time Manually")) {
            common->area = manual_area;
            common->location = manual_location;

            // Validate manual input against known continents and cities
            if (valid_continents.find(common->area) == valid_continents.end()) {
                error_message = "ERROR: Invalid continent";
            }
            else {
                bool valid_city = false;
                for (const auto& city : common->cities) {
                    if (city == common->location) {
                        valid_city = true;
                        break;
                    }
                }
                if (!valid_city) {
                    error_message = "ERROR: Invalid city";
                }
                else {
                    common->data_ready = false; // Reset data ready flag
                    common->cities_ready = false; // Ensure cities are not ready
                    error_message.clear(); // Clear error message if valid
                }
            }
        }
        ImGui::PopStyleColor(3); // Reset button colors

        if (!error_message.empty()) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", error_message.c_str());
        }
    }
    else {
        // Set button color for "Fetch Continents"
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));
        if (!common->continents_ready) {
            if (ImGui::Button("Fetch Continents")) {
                common->data_ready = false; // Reset data ready flag
                common->cities_ready = false; // Reset cities ready flag
                common->continents_ready = false; // Reset continents ready flag
            }
        }
        ImGui::PopStyleColor(3); // Reset button colors

        if (common->continents_ready) {
            static int selected_continent = -1;
            static int selected_city = -1; // Move city selection reset here

            // Set dropdown colors for continents
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 50, 255)); // Set dropdown background color
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(70, 70, 70, 255));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(90, 90, 90, 255));
            ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(50, 50, 50, 255)); // Set popup background color
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); // Set text color

            if (ImGui::BeginCombo("Continents", selected_continent == -1 ? "Select a continent" : common->continents[selected_continent].c_str())) {
                for (int i = 0; i < common->continents.size(); ++i) {
                    bool is_selected = (selected_continent == i);
                    if (ImGui::Selectable(common->continents[i].c_str(), is_selected)) {
                        if (selected_continent != i) {
                            selected_continent = i;
                            common->cities.clear(); // Clear the previous cities
                            common->cities_ready = false; // Reset cities ready flag
                            common->area = common->continents[selected_continent];
                            common->data_ready = false; // Reset data ready flag
                            selected_city = -1; // Reset city selection
                        }
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopStyleColor(5); // Reset dropdown colors

            // Set button color for "Fetch Cities"
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));
            if (selected_continent != -1 && !common->cities_ready) {
                if (ImGui::Button("Fetch Cities")) {
                    common->area = common->continents[selected_continent];
                    common->data_ready = false; // Reset data ready flag
                }
            }
            ImGui::PopStyleColor(3); // Reset button colors

            if (common->cities_ready) {
                // Set dropdown colors for cities
                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 50, 255)); // Set dropdown background color
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(70, 70, 70, 255));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(90, 90, 90, 255));
                ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(50, 50, 50, 255)); // Set popup background color
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); // Set text color

                if (ImGui::BeginCombo("Cities", selected_city == -1 ? "Select a city" : common->cities[selected_city].c_str())) {
                    for (int i = 0; i < common->cities.size(); ++i) {
                        bool is_selected = (selected_city == i);
                        if (ImGui::Selectable(common->cities[i].c_str(), is_selected)) {
                            selected_city = i;
                            common->location = common->cities[selected_city];
                            common->data_ready = false; // Reset data ready flag
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopStyleColor(5); // Reset dropdown colors

                // Set button color for "Fetch Time"
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));
                if (selected_city != -1 && ImGui::Button("Fetch Time")) {
                    common->data_ready = false; // Reset data ready flag
                }
                ImGui::PopStyleColor(3); // Reset button colors
            }
        }
    }

    if (common->data_ready) {
        ImGui::Text("Timezone: %s", common->timezone.c_str());
        ImGui::Text("UTC Offset: %s", common->utc_offset.c_str());

        // Set button color for "Use Digital Clock"/"Use Analog Clock"
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));
        if (ImGui::Button(use_analog_clock ? "Use Digital Clock" : "Use Analog Clock")) {
            use_analog_clock = !use_analog_clock;
        }
        ImGui::PopStyleColor(3); // Reset button colors

        ImGui::Separator();

        // Map display size to internal size (1 to 5 -> 6 to 10)
        float display_size_min = 1.0f;
        float display_size_max = 5.0f;
        float internal_size_min = 6.0f;
        float internal_size_max = 10.0f;
        display_size = ((clock_size - internal_size_min) / (internal_size_max - internal_size_min)) * (display_size_max - display_size_min) + display_size_min;

        // Set slider background color for "Clock Size"
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 50, 255)); // Set slider background color
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(70, 70, 70, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(90, 90, 90, 255));
        if (ImGui::SliderFloat("Clock Size", &display_size, display_size_min, display_size_max, "%.0f")) {
            // Map display size back to internal size
            clock_size = ((display_size - display_size_min) / (display_size_max - display_size_min)) * (internal_size_max - internal_size_min) + internal_size_min;
        }
        ImGui::PopStyleColor(3); // Reset slider colors

        ImGui::Separator();

        // Draw the clock
        ImGui::BeginChild("ClockChild", ImVec2(0, 0), false); // Create a child window for the clock
        ImVec2 child_pos = ImGui::GetCursorPos(); // Get the position of the child window
        ImVec2 child_size = ImGui::GetContentRegionAvail(); // Get the size available for the child window
        float rect_width = child_size.x; // Adjust rect width to fit the available space
        float rect_height = child_size.y; // Adjust rect height to fit the available space
        float center_x = (child_size.x - rect_width) * 0.5f; // Center the clock horizontally in the child window
        float center_y = (child_size.y - rect_height) * 0.5f; // Center the clock vertically in the child window
        ImGui::SetCursorPos(ImVec2(center_x, center_y)); // Set the cursor position to the center of the child window
        float radius = 10.0f * clock_size; // Adjust clock size based on slider
        if (use_analog_clock) {
            DrawAnalogClock(*common, radius); // Display the analog clock
        }
        else {
            DrawDigitalClock(*common, clock_size); // Display the digital clock
        }
        ImGui::EndChild(); // End the child window
    }
    else {
        ImGui::Text("Fetching data...");
    }
    ImGui::End();
}

void DrawThread::operator()(CommonObjects& common)
{
    GuiMain(DrawAppWindow, &common);
    common.exit_flag = true;
}
