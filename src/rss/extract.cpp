#include "extract.hpp"

#include <iostream>

#include <pugixml.hpp>
#include <fmt/format.h>

namespace rss::extract
{
    rss::feed parse(const std::string &filename)
    {
        rss::feed feed;

        fmt::print("Parsing: {}\n", filename);
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result)
        {
            return feed;
        }

        fmt::print("RSS Version: {}\n",
                doc.child("rss").attribute("version").value());

        pugi::xml_node channel = doc.child("rss").child("channel");

        feed.parsed = true;
        feed.title = channel.child("title").child_value();
        feed.link = channel.child("link").child_value();
        feed.description = channel.child("description").child_value();
        feed.language = channel.child("language").child_value();

        for (pugi::xml_node item : channel.children("item"))
        {
            feed_item feeditem;

            feeditem.title = item.child("title").child_value();
            feeditem.link = item.child("link").child_value();
            feeditem.guid = item.child("guid").child_value();
            feeditem.description = item.child("description").child_value();
            feeditem.pub_date = item.child("pubDate").child_value();

            feed.new_item(feeditem);
        }

        return feed;
    }
}
