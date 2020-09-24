#pragma once

#include <string>
#include "feed.hpp"

namespace rss::extract
{
    rss::feed parse(const std::string &filename);   
}

