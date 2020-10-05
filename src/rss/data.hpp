#pragma once

#include <filesystem>
#include <string_view>
#include <vector>
#include "feed.hpp"

namespace rss
{
    struct data
    {
        const std::string tmp_path = std::filesystem::temp_directory_path();
        std::string home_dir;
        std::string config_dir;
        std::vector<rss::feed> feeds_list;
        std::string browser = "xdg-open";
        bool load_cjk = true;

        data();
        ~data() = default;

        void load(std::string_view filepath);
        void save(std::string_view filepath);

        void load_fb(std::string_view filepath);
        void save_fb(std::string_view filepath);

        void add(const rss::feed &new_feed);
        void new_feed(std::string_view url, std::string filename = "");
    };
}

