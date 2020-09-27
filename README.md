# mfeed
Mart's (Graphical) RSS Feed Reader written in C++17/20 + imgui

## Requirements
* C++17/20 and C
* [cmake](https://cmake.org/)

### Dependencies
* [libcurl](https://curl.haxx.se/libcurl/)
* [glfw](https://www.glfw.org/)

#### Embedded
These are already embedded in the project
* [imgui](https://github.com/ocornut/imgui)
* [glad](https://glad.dav1d.de/)

#### cmake FetchContent
cmake just automaically will just fetch them so no actions needed
* [fmt](https://github.com/fmtlib/fmt)
* [pugixml](https://github.com/zeux/pugixml)
* [date](https://github.com/HowardHinnant/date)
* [json](https://github.com/nlohmann/json)

## Build
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

