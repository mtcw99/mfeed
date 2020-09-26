#include "data.hpp"

#include <optional>
#include <fstream>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "downloader.hpp"
#include "extract.hpp"
#include "feed.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

namespace rss
{
    data::data()
    {
        const char *home_dir = nullptr;

        if ((home_dir = getenv("HOME")) == nullptr)
        {
            home_dir = getpwuid(getuid())->pw_dir;
        }

        this->home_dir = home_dir;
    }

    void data::load(std::string_view filepath)
    {
        const std::string load_path = this->home_dir + '/' + filepath.data();
        nlohmann::json json;

        // Load json file
        std::ifstream load_file(load_path);
        load_file >> json;

        this->feeds_list.clear();
        for (auto &j_feed : json["feeds_list"])
        {
            rss::feed feed(j_feed);
            this->feeds_list.push_back(feed);
        }
    }

    void data::save(std::string_view filepath)
    {
        const std::string save_path = this->home_dir + '/' + filepath.data();
        nlohmann::json json;

        json["feeds_list"] = {};
        for (auto &feed : this->feeds_list)
        {
            json["feeds_list"].push_back(feed.to_json());
        }

        // Save to file
        std::ofstream save_file(save_path);
        save_file << json << std::endl;

        fmt::print("Saved to: {}\n", save_path);
    }

    void data::add(const rss::feed &new_feed)
    {
        this->feeds_list.push_back(new_feed);
    }

    void data::new_feed(std::string_view url, std::string_view filename)
    {
        const std::string filepath = this->tmp_path + '/' + filename.data();
        if (!std::filesystem::exists(filepath))
        {
            rss::downloader::fetch(url, filepath);
        }

        std::optional<rss::feed> feed_opt = rss::extract::parse(filepath);
        if (feed_opt.has_value())
        {
            rss::feed &feed = feed_opt.value();
            feed.url = url;
            feed.tmp_path = filepath;
            this->add(feed);
        }
    }
}

