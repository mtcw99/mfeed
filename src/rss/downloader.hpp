#pragma once

#include <string_view>

namespace rss::downloader
{
    void fetch(std::string_view url, std::string_view filename);
}

