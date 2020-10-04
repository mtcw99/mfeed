#include "data.hpp"

#include <optional>
#include <fstream>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include "flatbuffers/rss_data_generated.h"

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
        if (!std::filesystem::exists(load_path))
        {
            return;
        }
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
        this->browser = json["browser"];
    }

    void data::save(std::string_view filepath)
    {
        const std::string save_path = this->home_dir + '/' + filepath.data();
        nlohmann::json json;

        json["feeds_list"] = {};
        for (auto &feed : this->feeds_list)
        {
            if (!feed.erase)
            {
                json["feeds_list"].push_back(feed.to_json());
            }
        }
        json["browser"] = this->browser;

        // Save to file
        std::ofstream save_file(save_path);
        save_file << json << std::endl;

        fmt::print("Saved to: {}\n", save_path);
    }

    void data::load_fb(std::string_view filepath)
    {
        const std::string load_path = this->home_dir + '/' + filepath.data();
        if (!std::filesystem::exists(load_path))
        {
            return;
        }

    }

    void data::save_fb(std::string_view filepath)
    {
        const std::string save_path = this->home_dir + '/' + filepath.data();

        flatbuffers::FlatBufferBuilder builder;

        std::vector<flatbuffers::Offset<mfeed_fb::rss_data::Feed>> vec_feeds_list;
        for (auto &feed : this->feeds_list)
        {
            if (!feed.erase)
            {
                vec_feeds_list.push_back(feed.to_flatbuffer(builder));
            }
        }
        auto fb_browser = builder.CreateString(this->browser);

        auto fb_root = mfeed_fb::rss_data::CreateRSSData(builder,
                builder.CreateVector(vec_feeds_list),
                fb_browser);
        builder.Finish(fb_root);
    }

    void data::add(const rss::feed &new_feed)
    {
        this->feeds_list.push_back(new_feed);
    }

    static std::string url_filename(std::string_view url)
    {
        std::string filename;

        for (auto &ch : url)
        {
            switch (ch)
            {
            case ':':
            case '.':
            case '/':
            case '@':
                break;
            default:
                filename += ch;
            }
        }
        filename += ".rss";

        return filename;
    }

    void data::new_feed(std::string_view url, std::string filename)
    {
        if (filename == "")
        {
            filename = url_filename(url);
        }
        //fmt::print("filename: {}\n", filename);

        const std::string filepath = this->tmp_path + '/' + filename.c_str();
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

