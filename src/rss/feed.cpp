#include "feed.hpp"

namespace rss
{
    feed::feed()
    {
        this->parsed = false;
    }

    feed::~feed()
    {
    }

    void feed::new_item(feed_item &item)
    {
        this->items.push_back(item);
    }
}

