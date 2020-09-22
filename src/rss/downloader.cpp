#include <fmt/format.h>
#include <curl/curl.h>
#include <fstream>
#include "downloader.hpp"

namespace rss::downloader
{
    void print()
    {
        fmt::print("Test {}\n", "rss::downloader::print");
    }

    static int progress_func(void *,
            curl_off_t dltotal, curl_off_t dlnow,
            curl_off_t, curl_off_t)
    {
        fmt::print("{} of {} bytes received\r",
                dlnow, dltotal);
        return 0;
    }

    static size_t write_func(char *data, size_t size, size_t nmemb,
            void *v_stream)
    {
        if (v_stream == nullptr) return 0;
        auto stream = static_cast<std::ofstream *>(v_stream);
        stream->write(data, size * nmemb);
        return size * nmemb;
    }

    void fetch(const std::string &url, const std::string &filename)
    {
        fmt::print("Pulling: {}\n", url);
        CURL        *curl = nullptr;
        CURLcode    res;
        std::ofstream of(filename);

        curl = curl_easy_init();
        if (curl)
        {
            // Setup
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_func);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &of);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                fmt::print(stderr, "curl_easy_perform() failed: {}\n",
                        curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
        }

        fmt::print("Pulled: {}\n", url);
    }
}

