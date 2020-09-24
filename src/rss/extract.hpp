#pragma once

#include <string_view>
#include <optional>

#include "feed.hpp"

namespace rss::extract
{
    std::optional<rss::feed> parse(std::string_view filename);   
}

