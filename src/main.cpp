#include <string>
#include <filesystem>

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

    rss::feed feed = rss::extract::parse(filepath);
    if (feed.parsed)
    {
        fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
                feed.title, feed.link, feed.description, feed.language);

        for (auto &item : feed.items)
        {
            fmt::print("\nTitle: {}\nLink: {}\nDescription: {}\n",
                    item.title, item.link, item.description);
        }
    }

    return 0;
}

