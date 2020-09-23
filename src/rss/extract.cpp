#include "extract.hpp"

#include <iostream>
#include "pugixml.hpp"

namespace rss::extract
{
    void parse(const std::string &filename)
    {
        pugi::xml_document doc;
        std::cout << "Parsing: " << filename << '\n';
    }
}

