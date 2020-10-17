#include "main_ui.hpp"

#include "../rss/link.hpp"

#include <cstring>
#include <thread>
#include <future>

#include "date/date.h"
#include <fmt/format.h>

#include "imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

namespace gui
{
    main_ui::main_ui(const float scale_size,
            rss::data &data,
            GLFWwindow *&window) : data(data), window(window)
    {
        this->windows[static_cast<uint32_t>(main_ui_windows::side_feeds)].visible = true;
        this->windows[static_cast<uint32_t>(main_ui_windows::main_feed)].visible = true;

        this->windows[static_cast<uint32_t>(main_ui_windows::side_feeds)].func = std::bind(&gui::main_ui::side_feeds, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::main_feed)].func = std::bind(&gui::main_ui::main_feed, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::new_feed)].func = std::bind(&gui::main_ui::new_feed, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::feed_item)].func = std::bind(&gui::main_ui::feed_item, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::main_settings)].func = std::bind(&gui::main_ui::main_settings, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::feed_settings)].func = std::bind(&gui::main_ui::feed_settings, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::popup_remove_feed)].func = std::bind(&gui::main_ui::popup_remove_feed, this);
        this->windows[static_cast<uint32_t>(main_ui_windows::popup_clear_feed)].func = std::bind(&gui::main_ui::popup_clear_feed, this);

        this->mainbar_size = static_cast<uint32_t>(20 * scale_size);
    }

    void main_ui::windows_loop()
    {
        this->update();
        this->menubar();

        for (auto &window : this->windows)
        {
            if (window.visible)
            {
                window.func();
            }
        }
    }

    void main_ui::set_visibility(const main_ui_windows window,
            const bool visibility,
            const bool new_init)
    {
        this->windows[static_cast<uint32_t>(window)].visible = visibility;
        this->windows[static_cast<uint32_t>(window)].new_init = new_init;
    }

    bool main_ui::get_new_init(const main_ui_windows window)
    {
        return this->windows[static_cast<uint32_t>(window)].new_init;
    }

    void main_ui::set_new_init(const main_ui_windows window,
            const bool new_init)
    {
        this->windows[static_cast<uint32_t>(window)].new_init = new_init;
    }

    void main_ui::update()
    {
        this->sidebar_w = this->display_w / 4;
        this->sidebar_h = this->display_h - this->mainbar_size;
        this->main_w = this->display_w - this->sidebar_w;
        this->main_h = this->display_h - this->mainbar_size;
    }

    void main_ui::menubar()
    {
        // Main window menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Feed"))
                {
                    this->set_visibility(main_ui_windows::new_feed, true);
                }

                if (ImGui::MenuItem("Settings"))
                {
                    this->set_visibility(main_ui_windows::main_settings, true);
                    strcpy(browser, this->data.browser.c_str());
                }

                if (ImGui::MenuItem("Quit"))
                {
                    glfwSetWindowShouldClose(window, true);
                }

                if (ImGui::MenuItem("Quit without Saving"))
                {
                    this->save_on_exit = false;
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Feeds"))
            {
                if (ImGui::MenuItem("Update All"))
                {
                    std::thread{[this] {
                                for (auto &feed : this->data.feeds_list)
                                {
                                    std::thread{[&feed] {
                                                feed.update();
                                            }}.detach();
                                }
                            }}.detach();
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();
            ImGui::Text(fmt::format("Current: {}",
                        (focus_feed != nullptr) ? focus_feed->title : "None"
                        ).c_str());

#ifndef NDEBUG
            ImGui::Spacing();
            ImGui::Text(fmt::format("FPS: {}", fps).c_str());
            ImGui::Spacing();
            ImGui::Text(fmt::format("ms/f: {:.2f}", msf).c_str());
            ImGui::Spacing();
            ImGui::Text("NOTICE: Using debug build");
#else
            (void) fps;
            (void) msf;
#endif

            ImGui::EndMainMenuBar();
        }

    }

    void main_ui::side_feeds()
    {
        static char search_buffer[1024];

        ImGui::SetNextWindowPos(ImVec2(0, mainbar_size));
        ImGui::SetNextWindowSize(ImVec2(sidebar_w, sidebar_h));
        ImGui::Begin("Feeds");

        ImGui::InputText("Search", search_buffer, 1024);

        ImGui::Separator();

        if (ImGui::BeginChild("Feeds List"))
        {
            for (auto &feed : data.feeds_list)
            {
                if (!feed.erase &&
                        (feed.title.starts_with(search_buffer) ||
                         (feed.tags_check(search_buffer)) ||
                         (search_buffer[0] == '\0')))
                {
                    if (ImGui::Selectable((feed.title + " " +
                                    feed.tags_str()).c_str(),
                                (focus_feed == &feed)))
                    {
                        focus_feed = &feed;
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }

    void main_ui::main_feed()
    {
        ImGui::SetNextWindowPos(ImVec2(sidebar_w, mainbar_size));
        ImGui::SetNextWindowSize(ImVec2(main_w, main_h));
        ImGui::Begin("RSS Feed");

        if (focus_feed != nullptr)
        {
            if (ImGui::Button("Update"))
            {
                std::thread{[this]{
                            this->focus_feed->update();
                        }}.detach();
            }

            ImGui::SameLine();

            if (ImGui::Button("Setting"))
            {
                this->set_visibility(main_ui_windows::feed_settings, true, true);
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove"))
            {
                this->set_visibility(main_ui_windows::popup_remove_feed, true);
            }
            else
            {
                ImGui::SameLine();
                ImGui::Text(fmt::format("Updated: {} | Type: {}",
                            focus_feed->update_date_str(),
                            focus_feed->type_str()).c_str());
                ImGui::Text(focus_feed->title.c_str());
                if (ImGui::Button(focus_feed->link.c_str()))
                {
                    rss::link::open(focus_feed->link, data.browser);
                }
                if (focus_feed->description != "")
                {
                    ImGui::TextWrapped(focus_feed->description.c_str());
                }
                if (focus_feed->type == rss::e_feed_type::rss)
                {
                    ImGui::Text(focus_feed->language.c_str());
                }

                ImGui::Separator();

                for (auto &[i_key, item] : focus_feed->items)
                {
                    const std::string extraID = focus_feed->title + i_key;
                    if (ImGui::Button(("Read##" + extraID).c_str()))
                    {
                        focus_item = &item;
                        this->set_visibility(main_ui_windows::feed_item, true);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(("Open##" + extraID).c_str()))
                    {
                        rss::link::open(item.link, data.browser);
                    }
                    ImGui::SameLine();
                    ImGui::Text(item.pub_date_str().c_str());
                    ImGui::SameLine();
                    ImGui::TextWrapped(item.title.c_str());
                }
            }
        }

        ImGui::End();
    }

    void main_ui::new_feed()
    {
        static char url[512];
        ImGui::Begin("New Feed");

        ImGui::InputText("url", url, 512);

        if (ImGui::Button("Create"))
        {
            data.new_feed(url);
            url[0] = '\0';
            this->set_visibility(main_ui_windows::new_feed, false);
            focus_feed = nullptr;
        }

        if (ImGui::Button("Close"))
        {
            url[0] = '\0';
            this->set_visibility(main_ui_windows::new_feed, false);
        }

        ImGui::End();
    }

    void main_ui::feed_item()
    {
        ImGui::Begin("Item");

        if (ImGui::Button("Close"))
        {
            this->set_visibility(main_ui_windows::feed_item, false);
        }

        ImGui::SameLine();

        if (ImGui::Button("Open"))
        {
            rss::link::open(focus_item->link, data.browser);
        }

        for (const auto &line : focus_feed->open_with)
        {
            ImGui::SameLine();
            if (ImGui::Button(("Open (" + line + ")").c_str()))
            {
                rss::link::open(focus_item->link, line);
            }
        }

        ImGui::Text(focus_item->title.c_str());
        ImGui::Text(focus_item->link.c_str());
        ImGui::Text(focus_item->pub_date_str().c_str());

        ImGui::Separator();

        ImGui::TextWrapped(focus_item->description.c_str());

        ImGui::End();
    }

    void main_ui::main_settings()
    {
        ImGui::Begin("Settings");

        if (ImGui::Button("Close"))
        {
            this->set_visibility(main_ui_windows::main_settings, false);
        }

        ImGui::SameLine();

        if (ImGui::Button("Save"))
        {
            data.browser = browser;
        }

        ImGui::InputText("Browser", browser, 256);

        ImGui::End();
    }

    void main_ui::feed_settings()
    {
        static char openas_buffer[1024];
        static char tags_buffer[1024];
        static char title[512];
        ImGui::Begin("Feed Settings");

        if (this->get_new_init(main_ui_windows::feed_settings))
        {   // When this window pops up as new
            // Open As...
            std::string openas_buffer_str = "";
            for (const auto &str : focus_feed->open_with)
            {
                openas_buffer_str += str + '\n';
            }
            strcpy(openas_buffer, openas_buffer_str.c_str());

            // Tags
            strcpy(tags_buffer, focus_feed->tags_nl_str().c_str());

            // title
            strcpy(title, focus_feed->title.c_str());

            this->set_new_init(main_ui_windows::feed_settings, false);
        }

        if (ImGui::Button("Close"))
        {
            this->set_visibility(main_ui_windows::feed_settings, false);
            openas_buffer[0] = '\0';
            tags_buffer[0] = '\0';
            title[0] = '\0';
        }

        ImGui::SameLine();

        if (ImGui::Button("Save"))
        {
            // Open As...
            std::string line;
            std::stringstream ss(openas_buffer);
            focus_feed->open_with.clear();
            while (std::getline(ss, line))
            {
                focus_feed->open_with.push_back(line);
            }

            // Tags
            line = "";
            ss = std::stringstream(tags_buffer);
            focus_feed->tags.clear();
            while (std::getline(ss, line))
            {
                if (line != "")
                {
                    focus_feed->tags.push_back(line);
                }
            }

            // Title
            focus_feed->title = title;

            this->set_visibility(main_ui_windows::feed_settings, false);
            openas_buffer[0] = '\0';
            tags_buffer[0] = '\0';
            title[0] = '\0';
        }

        ImGui::InputText("Title", title, 512);
        ImGui::InputTextMultiline("Open as...", openas_buffer, 1024);
        ImGui::InputTextMultiline("Tags", tags_buffer, 1024);

        if (ImGui::Button("Clear Feed"))
        {
            this->set_visibility(main_ui_windows::popup_clear_feed, true);
        }

        if (ImGui::Button("Go to feed content"))
        {
            rss::link::open(focus_feed->url, data.browser);
        }

        ImGui::End();
    }

    void main_ui::popup_remove_feed()
    {
        ImGui::Begin("Removing feed");

        ImGui::Text(fmt::format("Remove \"{}\"?", focus_feed->title).c_str());

        if (ImGui::Button("Yes (Remove feed)"))
        {
            focus_feed->erase = true;
            focus_feed = nullptr;
            this->set_visibility(main_ui_windows::popup_remove_feed, false);
        }

        if (ImGui::Button("No (Do not remove feed)"))
        {
            this->set_visibility(main_ui_windows::popup_remove_feed, false);
        }

        ImGui::End();
    }

    void main_ui::popup_clear_feed()
    {
        ImGui::Begin("Clear Feed");

        ImGui::Text(fmt::format("Clear \"{}\"?", focus_feed->title).c_str());

        if (ImGui::Button("Yes (Clear feed)"))
        {
            focus_feed->items.clear();
            this->set_visibility(main_ui_windows::popup_clear_feed, false);
        }

        if (ImGui::Button("No (Do not clear feed)"))
        {
            this->set_visibility(main_ui_windows::popup_clear_feed, false);
        }

        ImGui::End();
    }
}

