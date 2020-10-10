#pragma once

#include "../rss/data.hpp"
#include "../rss/feed.hpp"

#include <functional>

// GUI
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace gui
{
    enum struct main_ui_windows : uint32_t {
        side_feeds = 0,
        main_feed,
        new_feed,
        feed_item,
        main_settings,
        feed_settings,
        popup_remove_feed,

        total
    };

    struct main_ui_window_info {
        bool visible = false;
        bool new_init = false;
        std::function<void()> func;
    };

    struct main_ui
    {
        rss::data &data;
        GLFWwindow *&window;

        main_ui_window_info windows[static_cast<uint32_t>(main_ui_windows::total)];
        bool save_on_exit = true;
        int display_w, display_h;
        char browser[256];
        uint32_t fps = 0;
        double msf = 0.0;
        uint32_t scale_size;
        rss::feed *focus_feed = nullptr;
        rss::feed_item *focus_item = nullptr;

        uint32_t mainbar_size;
        uint32_t sidebar_w = display_w / 4;
        uint32_t sidebar_h = display_h - mainbar_size;
        uint32_t main_w = display_w - sidebar_w;
        uint32_t main_h = display_h - mainbar_size;

        main_ui(const float scale_size,
                rss::data &data,
                GLFWwindow *&window);
        ~main_ui() = default;

        void windows_loop();

        void set_visibility(const main_ui_windows window,
                const bool visibility,
                const bool new_init = false);
        bool get_new_init(const main_ui_windows window);
        void set_new_init(const main_ui_windows window,
                const bool new_init);

        void update();

        void menubar();
        void side_feeds();
        void main_feed();

        void new_feed();
        void feed_item();
        void main_settings();
        void feed_settings();

        void popup_remove_feed();
    };
}

