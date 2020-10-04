#include "data.hpp"

#include <optional>
#include <fstream>
#include <memory>
#include <filesystem>

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
        const char *config_dir = nullptr;

        if ((home_dir = getenv("HOME")) == nullptr)
        {
            home_dir = getpwuid(getuid())->pw_dir;
        }

        this->home_dir = home_dir;

        if ((config_dir = getenv("XDG_CONFIG_HOME")) == nullptr ||
                config_dir[0] == '\0')
        {
            this->config_dir = this->home_dir + "/.config";
        }
        else
        {
            this->config_dir = config_dir;
        }

        this->config_dir += "/mfeed";

        if (!std::filesystem::exists(this->config_dir))
        {
            std::filesystem::create_directories(this->config_dir);
        }

        fmt::print("Config dir: {}\n", this->config_dir);
    }

    void data::load(std::string_view filepath)
    {
        const std::string load_path = this->config_dir + '/' + filepath.data();
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
        const std::string save_path = this->config_dir + '/' + filepath.data();
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
        const std::string load_path = this->config_dir + '/' + filepath.data();
        if (!std::filesystem::exists(load_path))
        {
            return;
        }

        std::ifstream in_file;
        in_file.open(load_path, std::ios::binary | std::ios::in);
        in_file.seekg(0, std::ios::end);
        uint32_t length = in_file.tellg();
        in_file.seekg(0, std::ios::beg);
        std::unique_ptr<char []> data = std::make_unique<char []>(length);
        in_file.read(data.get(), length);
        in_file.close();

        auto rss_data = mfeed_fb::rss_data::GetRSSData(data.get());

        auto fb_feeds_list = rss_data->feeds_list();
        for (uint32_t i = 0; i < fb_feeds_list->size(); ++i)
        {
            rss::feed feed(fb_feeds_list->Get(i));
            this->feeds_list.push_back(feed);
        }

        this->browser = rss_data->browser()->str();

        fmt::print("Loaded: {} from {}\n", length, load_path);
    }

    void data::save_fb(std::string_view filepath)
    {
        const std::string save_path = this->config_dir + '/' + filepath.data();

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

        builder.Finish(mfeed_fb::rss_data::CreateRSSData(builder,
                builder.CreateVector(vec_feeds_list),
                fb_browser));

        uint8_t *fb_buf = builder.GetBufferPointer();
        uint32_t size = builder.GetSize();

        std::ofstream out_file;
        out_file.open(save_path, std::ios::binary | std::ios::out);
        out_file.write(reinterpret_cast<const char *>(fb_buf), size);
        out_file.close();
        fmt::print("Written: {} bytes to: {}\n", size, save_path);
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

