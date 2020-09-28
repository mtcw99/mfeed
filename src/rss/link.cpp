#include "link.hpp"

#include <fmt/format.h>

namespace rss::link
{
    void open(std::string_view url, std::string_view with)
    {
        fmt::print("Opening \"{}\" with {}\n", url, with);
        system(fmt::format("{} \"{}\"", with, url).c_str());
    }
}

