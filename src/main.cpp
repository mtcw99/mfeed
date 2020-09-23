#include <string>
#include <filesystem>
#include <iostream>
#include "rss/downloader.hpp"
#include "rss/extract.hpp"

namespace fs = std::filesystem;

int main(int /*argc*/, char ** /*argv*/)
{
    const std::string tmp_path = fs::temp_directory_path();
    const std::string filepath = tmp_path + "/phoronix.rss";
    rss::downloader::fetch("https://www.phoronix.com/rss.php", filepath);
    rss::extract::parse(filepath);
    return 0;
}

