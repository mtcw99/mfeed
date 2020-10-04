#include "feed.hpp"

#include <fmt/format.h>

#include <optional>
#include <sstream>
#include <date/date.h>

#include "extract.hpp"
#include "downloader.hpp"

namespace rss
{
    // feed_item

    nlohmann::json feed_item::to_json()
    {
        nlohmann::json json;
        json["title"] = this->title;
        json["link"] = this->link;
        json["guid"] = this->guid;
        json["description"] = this->description;

        std::stringstream os_pubdate;
        os_pubdate << date::format("%F %T %z", this->pub_date);
        json["pub_date"] = os_pubdate.str();
        return json;
    }

    feed_item::feed_item(nlohmann::json json)
    {
        this->title = json["title"];
        this->link = json["link"];
        this->guid = json["guid"];
        this->description = json["description"];

        const std::string s_pubdate = json["pub_date"];
        std::stringstream is_pubdate(s_pubdate);
        is_pubdate >> date::parse("%F %T %z", this->pub_date);
    }

    std::string feed_item::pub_date_str()
    {
        std::stringstream os_pubdate;
        os_pubdate << date::format("%F %T", this->pub_date);
        return os_pubdate.str();
    }

    std::string feed_item::key()
    {
        return this->pub_date_str() + this->guid;
    }

    flatbuffers::Offset<mfeed_fb::rss_data::Item>
    feed_item::to_flatbuffer(flatbuffers::FlatBufferBuilder &fbb)
    {
        return mfeed_fb::rss_data::CreateItem(fbb,
                fbb.CreateString(this->title),
                fbb.CreateString(this->link),
                fbb.CreateString(this->guid),
                fbb.CreateString(this->description),
                fbb.CreateString(this->pub_date_str()));
    }

    feed_item::feed_item(const mfeed_fb::rss_data::Item *fb_item)
    {
        this->title = fb_item->title()->str();
        this->link = fb_item->link()->str();
        this->guid = fb_item->guid()->str();
        this->description = fb_item->description()->str();

        std::stringstream is_pubdate(fb_item->pub_date()->str());
        is_pubdate >> date::parse("%F %T", this->pub_date);
    }

    // feed

    void feed::new_item(feed_item &item)
    {
        this->items[item.key()] = item;
    }

    nlohmann::json feed::to_json()
    {
        nlohmann::json json;
        json["url"] = this->url;
        json["tmp_path"] = this->tmp_path;
        json["title"] = this->title;
        json["link"] = this->link;
        json["description"] = this->description;
        json["language"] = this->language;
        json["items"] = {};
        for (auto &[key, value] : this->items)
        {
            json["items"].push_back(value.to_json());
        }
        json["open_with"] = {};
        for (auto &line : this->open_with)
        {
            json["open_with"].push_back(line);
        }
        json["tags"] = {};
        for (auto &tag : this->tags)
        {
            json["tags"].push_back(tag);
        }
        return json;
    }

    feed::feed(nlohmann::json json)
    {
        this->url = json["url"];
        this->tmp_path = json["tmp_path"];
        this->title = json["title"];
        this->link = json["link"];
        this->description = json["description"];
        this->language = json["language"];
        for (const auto &j_item : json["items"])
        {
            feed_item f_item(j_item);
            this->items[f_item.key()] = f_item;
        }
        for (const auto &line : json["open_with"])
        {
            this->open_with.push_back(line);
        }
        for (const auto &tag : json["tags"])
        {
            this->tags.push_back(tag);
        }
    }

    void feed::update()
    {
        fmt::print("Update {} {}\n", this->url, this->tmp_path);
        rss::downloader::fetch(this->url, this->tmp_path);
        std::optional<rss::feed> feed_opt = rss::extract::parse(this->tmp_path);
        if (feed_opt.has_value())
        {
            rss::feed &feed = feed_opt.value();
            for (const auto &[key, value] : feed.items)
            {
                this->items[key] = value;
            }
        }
        else
        {
            fmt::print(stderr, "ERROR: Failed to parse file.\n");
        }
    }

    std::string feed::tags_str()
    {
        if (this->tags.size() == 0)
        {
            return "";
        }

        std::string str = "(";

        for (const auto &tag : this->tags)
        {
            if (&tag == &this->tags.back())
            {
                str += tag + ")";
            }
            else
            {
                str += tag + ",";
            }
        }

        return str;
    }

    std::string feed::tags_nl_str()
    {
        if (this->tags.size() == 0)
        {
            return "";
        }

        std::string str = "";

        for (const auto &tag : this->tags)
        {
            str += tag + '\n';
        }

        return str;
    }

    bool feed::tags_check(std::string_view search_buffer)
    {
        for (const auto &tag : this->tags)
        {
            if (tag.starts_with(search_buffer))
            {
                return true;
            }
        }

        return false;
    }

    flatbuffers::Offset<mfeed_fb::rss_data::Feed>
    feed::to_flatbuffer(flatbuffers::FlatBufferBuilder &fbb)
    {
        std::vector<flatbuffers::Offset<mfeed_fb::rss_data::Item>> vec_items;
        for (auto &[key, value] : this->items)
        {
            vec_items.push_back(value.to_flatbuffer(fbb));
        }

        std::vector<flatbuffers::Offset<flatbuffers::String>> vec_open_with;
        for (auto &line : this->open_with)
        {
            vec_open_with.push_back(fbb.CreateString(line));
        }

        std::vector<flatbuffers::Offset<flatbuffers::String>> vec_tags;
        for (auto &tag : this->tags)
        {
            vec_tags.push_back(fbb.CreateString(tag));
        }

        return mfeed_fb::rss_data::CreateFeed(fbb,
                fbb.CreateString(this->url),
                fbb.CreateString(this->tmp_path),
                fbb.CreateString(this->title),
                fbb.CreateString(this->link),
                fbb.CreateString(this->description),
                fbb.CreateString(this->language),
                fbb.CreateVector(vec_items),
                fbb.CreateVector(vec_open_with),
                fbb.CreateVector(vec_tags));
    }

    feed::feed(const mfeed_fb::rss_data::Feed *fb_feed)
    {
        this->url = fb_feed->url()->str();
        this->tmp_path = fb_feed->tmp_path()->str();
        this->title = fb_feed->title()->str();
        this->link = fb_feed->link()->str();
        this->description = fb_feed->description()->str();
        this->language = fb_feed->language()->str();

        auto fb_vec_items = fb_feed->items();
        for (uint32_t i = 0; i < fb_vec_items->size(); ++i)
        {
            feed_item f_item(fb_vec_items->Get(i));
            this->items[f_item.key()] = f_item;
        }

        auto fb_vec_open_with = fb_feed->open_with();
        for (uint32_t i = 0; i < fb_vec_open_with->size(); ++i)
        {
            this->open_with.push_back(fb_vec_open_with->Get(i)->str());
        }

        auto fb_vec_tags = fb_feed->tags();
        for (uint32_t i = 0; i < fb_vec_tags->size(); ++i)
        {
            this->tags.push_back(fb_vec_tags->Get(i)->str());
        }
    }
}

