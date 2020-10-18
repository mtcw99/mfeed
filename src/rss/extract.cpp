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
            feed.type = rss::e_feed_type::rss;

            for (pugi::xml_node item : channel.children("item"))
            {
                feed_item feeditem;

                feeditem.title = item.child("title").child_value();
                feeditem.link = item.child("link").child_value();
                feeditem.guid = item.child("guid").child_value();
                auto all_content = item.child("description").text().get();
                std::istringstream content_stream(all_content);
                for (std::string line; std::getline(content_stream, line); )
                {
                    feeditem.content.push_back(line);
                }

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
#if 0
            pugi::xml_node author = doc.child("feed").child("author");

            feed.title = author.child("name").child_value();
            feed.link = author.child("uri").child_value();
            feed.description = author.child("content").child_value();
            feed.language = "";
#endif
            pugi::xml_node feednode = doc.child("feed");

            feed.title = feednode.child("title").child_value();
            for (pugi::xml_node link : feednode.children("link"))
            {
                const std::string &rel = link.attribute("rel").value();
                if (rel == "alternate")
                {
                    feed.link = link.attribute("href").value();
                }
            }
            feed.description = feednode.child("content").child_value();
            if (feed.description == "")
            {
                feed.description = feednode.child("subtitle").child_value();
            }
            feed.language = "";
            feed.type = rss::e_feed_type::atom;

            for (pugi::xml_node item : doc.child("feed").children("entry"))
            {
                feed_item feeditem;

                feeditem.title = item.child("title").child_value();
                feeditem.link = item.child("link").attribute("href").value();
                feeditem.guid = item.child("id").child_value();

                auto all_content = item.child("summary").text().get();
                std::istringstream content_stream(all_content);
                for (std::string line; std::getline(content_stream, line); )
                {
                    feeditem.content.push_back(line);
                }

                if (all_content[0] == '\0' || feeditem.content.size() == 0)
                {
                    auto all_content = item.child("content").text().get();
                    std::istringstream content_stream(all_content);
                    for (std::string line; std::getline(content_stream, line); )
                    {
                        feeditem.content.push_back(line);
                    }
                }

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

    static void parse_content_traverse(pugi::xml_node node,
            std::vector<rss::extract::content> &html_content,
            const uint32_t level = 0)
    {
        uint32_t line_i = 0;
        for (pugi::xml_node item : node.children())
        {
            if (item.name()[0] != '\0')
            {
#if 0
                fmt::print("NODE ({}, {}): {}\n", level, line_i, item.name());
                fmt::print("CONTENT ({}, {}): {}\n", level, line_i, item.text().get());
                fmt::print("{}\n", item.text().get());
#else
                if (item.name()[0] == 'p')
                {
                    rss::extract::content next_content;
                    next_content.content = std::string(item.text().get());
                    html_content.push_back(next_content);
                }
                else if (item.name()[0] == 'a')
                {
                    rss::extract::content next_content;
                    next_content.type = e_content_type::hyperlink;
                    rss::extract::s_hyperlink hyperlink;
                    hyperlink.link = item.attribute("href").as_string();
                    for (const auto &ch_text : item.children())
                    {
                        hyperlink.text += ch_text.text().get();
                    }
                    next_content.content = hyperlink;
                    html_content.push_back(next_content);
                }
                else
                {
                    if (html_content.size() > 0 &&
                            html_content.back().type == e_content_type::paragraph)
                    {
                        std::get<std::string>(html_content.back().content) += item.text().get();
                    }
                    else
                    {
                        rss::extract::content next_content;
                        next_content.content = std::string(item.text().get());
                        html_content.push_back(next_content);
                    }
                }
#endif
            }
            if (item.name()[0] != 'a')
            {
                parse_content_traverse(item, html_content, level + 1);
            }
            ++line_i;
        }
    }

    std::vector<rss::extract::content> parse_content(const std::vector<std::string> &content)
    {
        std::vector<rss::extract::content> html_content;

        std::stringstream content_stream;
        for (const auto &line : content)
        {
            content_stream << line;
        }
        content_stream.seekp(0);

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load(content_stream);
        if (!result)
        {
            for (const auto &line : content)
            {
                rss::extract::content next_content;
                next_content.content = line;
                html_content.push_back(next_content);
            }
            return html_content;
        }

        parse_content_traverse(doc, html_content);
        return html_content;
    }
}

