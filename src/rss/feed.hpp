#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "date/date.h"       // Remove when gcc fully implements C++20 date feature
#include <nlohmann/json.hpp>

// C++20:
// - date:: TO std::chrono::

namespace rss
{
    struct feed_item
    {
        std::string title;
        std::string link;
        std::string guid;
        std::string description;
        date::sys_time<std::chrono::seconds> pub_date;

        feed_item() = default;
        feed_item(nlohmann::json json);

        nlohmann::json to_json();
    };

    struct feed
    {
        std::string url;
        std::string tmp_path;

        std::string title;
        std::string link;
        std::string description;
        std::string language;

        std::vector<feed_item> items;

        feed() = default;
        feed(nlohmann::json json);
        ~feed() = default;

        void new_item(feed_item &item);
        nlohmann::json to_json();
    };
}

