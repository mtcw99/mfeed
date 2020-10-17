#include "extract.hpp"

#include <iostream>
#include <sstream>

#include <pugixml.hpp>
#include <fmt/format.h>

namespace rss::extract
{
    std::optional<rss::feed> parse(std::string_view filename)
    {
        rss::feed feed;

        fmt::print("Parsing: {}\n", filename);
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filename.data());
        if (!result)
        {
            return {};
        }

        pugi::xml_node find_rss = doc.child("rss");
        pugi::xml_node find_atom = doc.child("feed");

        bool is_rss = (std::string(find_rss.name()) == "rss");
        bool is_atom = (std::string(find_atom.name()) == "feed");
        fmt::print("RSS: {} | Atom: {}\n",
                find_rss.name(), find_atom.name());

        if (is_rss)
        {   // RSS 2.0
#if 0
            fmt::print("RSS Version: {}\n",
                    doc.child("rss").attribute("version").value());
#endif
            pugi::xml_node channel = doc.child("rss").child("channel");

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

                // RFC 822 Timestamp
                std::istringstream pub_date_str(item.child("pubDate").child_value());
                pub_date_str >> date::parse("%a, %d %b %Y %T %z", feeditem.pub_date);
                const auto ymd = date::year_month_day{date::floor<date::days>(feeditem.pub_date)};
                if (ymd.year() == date::year{1970})
                {
                    pub_date_str = std::istringstream(item.child("pubDate").child_value());
                    pub_date_str >> date::parse("%a, %d %b %Y %T", feeditem.pub_date);
                }

                feed.new_item(feeditem);
            }
        }
        else if (is_atom)
        {   // Atom
            pugi::xml_node author = doc.child("feed").child("author");

            feed.title = author.child("name").child_value();
            feed.link = author.child("uri").child_value();
            feed.description = author.child("content").child_value();
            feed.language = "";

            for (pugi::xml_node item : doc.child("feed").children("entry"))
            {
                feed_item feeditem;

                feeditem.title = item.child("title").child_value();
                feeditem.link = item.child("link").attribute("href").value();
                feeditem.guid = item.child("id").child_value();
                feeditem.description = "";

                // RFC 3339 Timestamp
                if (std::string(item.child("published").name()) == "published")
                {
                    std::istringstream pub_date_str(item.child("published").child_value());
                    pub_date_str >> date::parse("%FT%T%Ez", feeditem.pub_date);
                    const auto ymd = date::year_month_day{date::floor<date::days>(feeditem.pub_date)};
                    if (ymd.year() == date::year{1970})
                    {
                        pub_date_str = std::istringstream(item.child("published").child_value());
                        pub_date_str >> date::parse("%FT%TZ", feeditem.pub_date);
                    }
                }
                else if (std::string(item.child("updated").name()) == "updated")
                {
                    std::istringstream pub_date_str(item.child("updated").child_value());
                    pub_date_str >> date::parse("%FT%T%Ez", feeditem.pub_date);
                    const auto ymd = date::year_month_day{date::floor<date::days>(feeditem.pub_date)};
                    if (ymd.year() == date::year{1970})
                    {
                        pub_date_str = std::istringstream(item.child("updated").child_value());
                        pub_date_str >> date::parse("%FT%TZ", feeditem.pub_date);
                    }
                }

                feed.new_item(feeditem);
            }
        }

        return feed;
    }
}

