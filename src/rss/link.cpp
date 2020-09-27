#include "link.hpp"

#include <fmt/format.h>

namespace rss::link
{
    void open(std::string_view url, std::string_view with)
    {
        fmt::print("open url: {}\n", url);
        system(fmt::format("{} {}", with, url).c_str());
    }
}

