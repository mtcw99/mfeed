#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
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
        std::string pub_date_str();
        std::string key();
    };

    struct feed
    {
        std::string url;
        std::string tmp_path;

        std::string title;
        std::string link;
        std::string description;
        std::string language;

        std::map<std::string, feed_item, std::greater<std::string>> items;
        std::vector<std::string> open_with;
        std::vector<std::string> tags;

        bool erase = false;

        feed() = default;
        feed(nlohmann::json json);
        ~feed() = default;

        void new_item(feed_item &item);
        nlohmann::json to_json();
        void update();
        std::string tags_str();
        std::string tags_nl_str();
        bool tags_check(std::string_view search_buffer);
    };
}

