#pragma once

#include <string>
#include <vector>

namespace rss
{
    struct feed_item
    {
        std::string title;
        std::string link;
        std::string guid;
        std::string description;
        std::string pub_date;
    };

    struct feed
    {
        std::string title;
        std::string link;
        std::string description;
        std::string language;
        bool        parsed;

        std::vector<feed_item> items;

        feed();
        ~feed();

        void new_item(feed_item &item);
    };
}

