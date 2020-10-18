#include "feed.hpp"

#include <fmt/format.h>

#include <optional>
#include <sstream>

#include "extract.hpp"
#include "downloader.hpp"

namespace rss
{
    // feed_item

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
        std::vector<flatbuffers::Offset<flatbuffers::String>> fb_content;
        for (const auto &line : this->content)
        {
            fb_content.push_back(fbb.CreateString(line));
        }

        return mfeed_fb::rss_data::CreateItem(fbb,
                fbb.CreateString(this->title),
                fbb.CreateString(this->link),
                fbb.CreateString(this->guid),
                fbb.CreateVector(fb_content),
                fbb.CreateString(this->pub_date_str()));
    }

    feed_item::feed_item(const mfeed_fb::rss_data::Item *fb_item)
    {
        this->title = fb_item->title()->str();
        this->link = fb_item->link()->str();
        this->guid = fb_item->guid()->str();
        auto fb_content = fb_item->content();
        for (uint32_t i = 0; i < fb_content->size(); ++i)
        {
            this->content.push_back(fb_content->Get(i)->str());
        }

        std::stringstream is_pubdate(fb_item->pub_date()->str());
        is_pubdate >> date::parse("%F %T", this->pub_date);
    }

    // feed

    void feed::new_item(feed_item &item)
    {
        this->items[item.key()] = item;
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
            this->update_date = date::floor<std::chrono::seconds>(
                    std::chrono::system_clock::now());

            this->link = feed.link;
            this->description = feed.description;
            this->language = feed.language;
            this->type = feed.type;
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

    std::string feed::update_date_str()
    {
        std::stringstream os_updatedate;
        os_updatedate << date::format("%F %T", this->update_date);
        return os_updatedate.str();
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
                fbb.CreateVector(vec_tags),
                fbb.CreateString(this->update_date_str()),
                this->type_to_flatbuffer());
    }

    feed::feed(const mfeed_fb::rss_data::Feed *fb_feed)
    {
        this->url = fb_feed->url()->str();
        this->tmp_path = fb_feed->tmp_path()->str();
        this->title = fb_feed->title()->str();
        this->link = fb_feed->link()->str();
        this->description = fb_feed->description()->str();
        this->language = fb_feed->language()->str();
        this->type_from_flatbuffer(fb_feed->type());
        if (fb_feed->update_date() != nullptr)
        {
            std::stringstream is_updatedate(fb_feed->update_date()->str());
            is_updatedate >> date::parse("%F %T", this->update_date);
        }

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

    std::string feed::type_str() const
    {
        return (this->type == rss::e_feed_type::rss) ? "rss" : "atom";
    }

    mfeed_fb::rss_data::FeedType feed::type_to_flatbuffer() const
    {
        switch (this->type)
        {
        case rss::e_feed_type::rss:
            return mfeed_fb::rss_data::FeedType_rss;
        case rss::e_feed_type::atom:
        default:
            return mfeed_fb::rss_data::FeedType_atom;
        }
    }

    void feed::type_from_flatbuffer(mfeed_fb::rss_data::FeedType fb_type)
    {
        switch (fb_type)
        {
        case mfeed_fb::rss_data::FeedType_rss:
            this->type = rss::e_feed_type::rss;
            break;
        case mfeed_fb::rss_data::FeedType_atom:
            this->type = rss::e_feed_type::atom;
            break;
        }
    }
}

