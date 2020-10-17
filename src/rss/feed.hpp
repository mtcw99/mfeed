#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include "date/date.h"       // Remove when gcc fully implements C++20 date feature
#include "flatbuffers/rss_data_generated.h"

// C++20:
// - date:: TO std::chrono::

namespace rss
{
    enum struct e_feed_type : int8_t
    {
        rss = 0,
        atom = 1
    };

    struct feed_item
    {
        std::string title;
        std::string link;
        std::string guid;
        std::string description;
        date::sys_time<std::chrono::seconds> pub_date;

        feed_item() = default;
        feed_item(const mfeed_fb::rss_data::Item *fb_item);

        std::string pub_date_str();
        std::string key();
        flatbuffers::Offset<mfeed_fb::rss_data::Item> to_flatbuffer(flatbuffers::FlatBufferBuilder &fbb);
    };

    struct feed
    {
        std::string url;
        std::string tmp_path;

        std::string title;
        std::string link;
        std::string description;
        std::string language;

        e_feed_type type;

        date::sys_time<std::chrono::seconds> update_date =
                date::floor<std::chrono::seconds>(
                        std::chrono::system_clock::now());

        std::map<std::string, feed_item, std::greater<std::string>> items;
        std::vector<std::string> open_with;
        std::vector<std::string> tags;

        bool erase = false;

        feed() = default;
        feed(const mfeed_fb::rss_data::Feed *fb_feed);
        ~feed() = default;

        void new_item(feed_item &item);
        void update();
        std::string tags_str();
        std::string tags_nl_str();
        bool tags_check(std::string_view search_buffer);
        flatbuffers::Offset<mfeed_fb::rss_data::Feed> to_flatbuffer(flatbuffers::FlatBufferBuilder &fbb);
        std::string update_date_str();
        std::string type_str() const;
        mfeed_fb::rss_data::FeedType type_to_flatbuffer() const;
        void type_from_flatbuffer(mfeed_fb::rss_data::FeedType fb_type);
    };
}

