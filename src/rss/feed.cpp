#include "feed.hpp"

namespace rss
{
    void feed::new_item(feed_item &item)
    {
        this->items.push_back(item);
    }
}

