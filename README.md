Project Title: Advanced C++ Course Final Project
Project Description
This project is a final assignment for the Advanced Programming in C++ course at the Jerusalem College of Engineering, supervised by Harel Walach and Dr. Eli Engelberg. The project demonstrates the use of C++ Standard Library (STL), multi-threading, synchronization, and integration of third-party libraries (httplib, json, imgui) to create an HTTP client with a graphical user interface (GUI).

Features
Fetches JSON data from an HTTP server using httplib.
Parses and processes JSON data using nlohmann::json.
Displays data using a GUI built with imgui.
Multi-threaded processing with std::thread and synchronization with std::atomic.
File Structure
mathematica
Copy code
.
├── .vs
│   └── CppApp
├── shared
│   ├── HttpSrc
│   │   ├── httplib.h
│   │   ├── nlohmann
│   │   │   ├── json.hpp
│   │   │   └── ...
│   │   └── openssl
│   │       └── ...
│   └── ImGuiSrc
│       └── ...
├── winapp
│   ├── ConnectedApp
│   │   ├── CommonObject.h
│   │   ├── ConnectedApp.cpp
│   │   ├── DownloadThread.cpp
│   │   ├── DownloadThread.h
│   │   ├── DrawThread.cpp
│   │   ├── DrawThread.h
│   │   └── ...
│   └── ImGuiLib
│       └── ...
├── x64
│   └── Debug
└── CppApp.sln
Installation Instructions
Clone the repository:

bash
Copy code
git clone <repository_url>
cd <repository_directory>
Install Dependencies:

Ensure you have CMake installed: CMake Download
Ensure you have a C++17 compatible compiler.
Install any necessary libraries (OpenSSL, ImGui, etc.).
Build the Project:

bash
Copy code
mkdir build
cd build
cmake ..
make
Usage Instructions
Run the Application:

bash
Copy code
./ConnectedApp
User Interface:

The application will display a GUI where you can interact with the downloaded data.
Select different options to view and process the data.
Detailed File Descriptions
ConnectedApp.cpp
The main entry point of the application:

Initializes common objects and threads for drawing and downloading.
Manages the workflow for fetching continents, cities, and data based on the common object's flags.
Ensures threads are properly joined before exiting.
CommonObject.h
Defines the CommonObjects struct:

Holds various atomic flags (data_ready, exit_flag, continents_ready, cities_ready) for thread synchronization.
Contains fields for storing fetched data (time_data, timezone, utc_offset, area, location, continents, cities).
DownloadThread.cpp / DownloadThread.h
Handles the download operations:

Uses httplib to make HTTP requests.
Parses JSON responses using nlohmann::json.
Fetches and processes data from an API.
DrawThread.cpp / DrawThread.h
Manages the graphical user interface:

Utilizes ImGui for rendering the GUI.
Draws elements such as digital clocks and other data visualizations.
Dependencies
C++17 Standard Library: Used for data structures and threading.
httplib: For HTTP client functionality.
nlohmann::json: For JSON parsing.
ImGui: For the graphical user interface.
OpenSSL: For secure HTTP connections.
Example API Usage
The application fetches data from various APIs, such as the World Time API:

World Time API: Fetches time zone data for a given location.
URL Example: http://worldtimeapi.org/api/timezone/Europe/London
JSON Response Example:
json
Copy code
{
  "datetime": "2024-08-04T14:30:00Z",
  "timezone": "Europe/London",
  "utc_offset": "+01:00"
}
Contributing
Fork the repository.
Create a new branch (git checkout -b feature-branch).
Commit your changes (git commit -am 'Add new feature').
Push to the branch (git push origin feature-branch).
Create a new Pull Request.
