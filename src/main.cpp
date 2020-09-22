#include <string>
#include "rss/downloader.hpp"

int main(int /*argc*/, char ** /*argv*/)
{
    rss::downloader::print();
    rss::downloader::fetch("https://www.phoronix.com/rss.php", "phoronix.rss");
    return 0;
}

