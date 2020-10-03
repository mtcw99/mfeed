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
}

