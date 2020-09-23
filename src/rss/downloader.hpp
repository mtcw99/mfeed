#pragma once

#include <string>

namespace rss::downloader
{
    void fetch(const std::string &url, const std::string &filename);
}

