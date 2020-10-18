#pragma once

#include <string_view>
#include <optional>
#include <vector>
#include <string>
#include <variant>

#include "feed.hpp"

// use std::variant?

namespace rss::extract
{
    enum struct e_content_type : uint32_t
    {
        paragraph,
        hyperlink,

        total
    };

    struct s_hyperlink
    {
        std::string link;
        std::string text;
    };

    struct content
    {
        e_content_type type = e_content_type::paragraph;
        std::variant<std::string, s_hyperlink> content;
    };

    std::optional<rss::feed> parse(std::string_view filename);   
    std::vector<rss::extract::content> parse_content(const std::vector<std::string> &content);
}

