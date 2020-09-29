#include "link.hpp"

#include <fmt/format.h>
#include <unistd.h>

namespace rss::link
{
    void open(std::string_view url, std::string_view with)
    {
        fmt::print("Opening \"{}\" with {}\n", url, with);
        if (fork() == 0)
        {
            execlp(with.data(), with.data(), url.data(), nullptr);
        }
    }
}

