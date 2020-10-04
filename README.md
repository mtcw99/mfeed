# mfeed
Mart's (Graphical) RSS Feed Reader written in C++20 + imgui

![2020-01-04 Screenshot of mfeed](screenshots/mfeed_2020-10-04.png)

## Requirements
* C++20 and C
  * C++20: Must have [starts_with](https://en.cppreference.com/w/cpp/string/basic_string/starts_with) in `std::string` 
* [cmake](https://cmake.org/)

### Dependencies
* [libcurl](https://curl.haxx.se/libcurl/)
* [glfw](https://www.glfw.org/)

#### Embedded
These are already embedded in the project
* [imgui](https://github.com/ocornut/imgui)
* [glad](https://glad.dav1d.de/)
* [flatbuffers](https://github.com/google/flatbuffers)

#### cmake FetchContent
cmake just automaically will just fetch them so no actions needed
* [fmt](https://github.com/fmtlib/fmt)
* [pugixml](https://github.com/zeux/pugixml)
* [date](https://github.com/HowardHinnant/date)
* [json](https://github.com/nlohmann/json)

### Future/Planned Requirements
* C++20
  * Modules support (Whenever that arrives for gcc and cmake)
  * C++20 `std::chrono` date and times (Currently [date](https://github.com/HowardHinnant/date) is in place of it
  * Maybe: `std::format` over [fmt](https://github.com/fmtlib/fmt)

## Build
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`

## License
mfeed is licensed under the [GPLv3 (GNU General Public License v3.0)](LICENSE) ([gnu.org link](https://www.gnu.org/licenses/gpl-3.0.html)).

