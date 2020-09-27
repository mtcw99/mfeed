#include "feed.hpp"

#include <sstream>
#include <date/date.h>

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

    // feed

    void feed::new_item(feed_item &item)
    {
        this->items.push_back(item);
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
        for (auto &item : this->items)
        {
            json["items"].push_back(item.to_json());
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
        for (auto &j_item : json["items"])
        {
            feed_item f_item(j_item);
            this->items.push_back(f_item);
        }
    }
}

