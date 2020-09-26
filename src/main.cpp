#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "date/date.h"
#include <fmt/format.h>

#include "rss/data.hpp"
#include "rss/feed.hpp"

int main(int /*argc*/, char ** /*argv*/)
{
    rss::data data;

#if 0
    data.new_feed("https://www.phoronix.com/rss.php", "phoronix.rss");

    rss::feed &feed = data.feeds_list[0];

    fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
            feed.title, feed.link, feed.description, feed.language);

    for (auto &item : feed.items)
    {
        std::stringstream os_pubdate;
        os_pubdate << date::format("%F %T", item.pub_date);
        fmt::print("\nTitle: {}\nDate: {}\nLink: {}\nDescription: {}\n",
                item.title, os_pubdate.str(), item.link, item.description);
    }

    data.save("test.json");
#endif
    data.load("test.json");

    for (auto &feed : data.feeds_list)
    {
        fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
                feed.title, feed.link, feed.description, feed.language);

        for (auto &item : feed.items)
        {
            std::stringstream os_pubdate;
            os_pubdate << date::format("%F %T", item.pub_date);
            fmt::print("\nTitle: {}\nDate: {}\nLink: {}\nDescription: {}\n",
                    item.title, os_pubdate.str(), item.link, item.description);
        }
    }

    std::cin.get();

    return 0;
}

