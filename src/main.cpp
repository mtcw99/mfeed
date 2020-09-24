#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "date/date.h"
#include <fmt/format.h>

#include "rss/downloader.hpp"
#include "rss/extract.hpp"

namespace fs = std::filesystem;

int main(int /*argc*/, char ** /*argv*/)
{
    const std::string tmp_path = fs::temp_directory_path();
    const std::string filepath = tmp_path + "/phoronix.rss";
    if (!fs::exists(filepath))
    {
        rss::downloader::fetch("https://www.phoronix.com/rss.php", filepath);
    }

    std::optional<rss::feed> feed_opt = rss::extract::parse(filepath);
    if (feed_opt.has_value())
    {
        rss::feed &feed = feed_opt.value();

        fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
                feed.title, feed.link, feed.description, feed.language);

        for (auto &item : feed.items)
        {
            std::stringstream os_pubdate;
            os_pubdate << date::format("%F %T", item.pub_date);
            fmt::print("\nTitle: {}\nLink: {}\nDescription: {}\nPubDate: {}\n",
                    item.title, item.link, item.description, os_pubdate.str());
        }
    }

    return 0;
}

