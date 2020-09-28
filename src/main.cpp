#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <future>

#include "date/date.h"
#include <fmt/format.h>

#include "rss/data.hpp"
#include "rss/feed.hpp"
#include "rss/link.hpp"

// GUI
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

const char *PROG_NAME = "mfeed";
const uint32_t SCR_WIDTH = 800;
const uint32_t SCR_HEIGHT = 600;
const bool SET_WIREFRAMEPOLY = false;
const char *GLSL_VERSION = "#version 330 core";

void processInput(GLFWwindow* window)
{
    switch(glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
    case GLFW_PRESS:
        glfwSetWindowShouldClose(window, true);
    }
}

void framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void glfw_error_callback(int32_t error, const char *description)
{
    fmt::print(stderr, "Glfw Error {} : {}\n", error, description);
}

int main(int /*argc*/, char ** /*argv*/)
{
    rss::data data;
    data.load("test.json");

    // glfw: initialize and configure
    //================================
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    //=====================
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, PROG_NAME,
            nullptr, nullptr);

    if (window == nullptr)
    {
        fmt::print(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    //========================================
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        fmt::print(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // IMGUI settings
    //===============
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    bool windows_newFeed = false;
    bool windows_feedItemContent = false;
    bool windows_settings = false;
    int display_w, display_h;
    char browser[256];
    double time_previous = glfwGetTime();
    uint32_t frame_count = 0;
    uint32_t fps = 0;
    double msf = 0.0;
    rss::feed *focus_feed = nullptr;
    rss::feed_item *focus_item = nullptr;

    while (!glfwWindowShouldClose(window))
    {
#if 1
        glfwWaitEvents();
#else
        glfwPollEvents();
#endif

        double time_current = glfwGetTime();
        ++frame_count;
        if (time_current - time_previous >= 1.0)
        {
            msf = static_cast<double>(1000.0 / double(frame_count));
            fps = static_cast<uint32_t>(1000.0 / msf);
            frame_count = 0;
            time_previous = time_current;
        }

        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window menu bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Feed"))
                {
                    windows_newFeed = true;
                }

                if (ImGui::MenuItem("Settings"))
                {
                    windows_settings = true;
                    strcpy(browser, data.browser.c_str());
                }

                if (ImGui::MenuItem("Quit"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();
            ImGui::Text(fmt::format("Current: {}",
                        (focus_feed != nullptr) ? focus_feed->title : "None"
                        ).c_str());
            ImGui::Spacing();
            ImGui::Text(fmt::format("FPS: {}", fps).c_str());
            ImGui::Spacing();
            ImGui::Text(fmt::format("ms/f: {:.2f}", msf).c_str());
            ImGui::EndMainMenuBar();
        }

        const uint32_t mainbar_size = 20;
        const uint32_t sidebar_w = display_w / 4;
        const uint32_t sidebar_h = display_h - mainbar_size;
        const uint32_t main_w = display_w - sidebar_w;
        const uint32_t main_h = display_h - mainbar_size;

        {   // RSS Feeds Sidebar
            ImGui::SetNextWindowPos(ImVec2(0, mainbar_size));
            ImGui::SetNextWindowSize(ImVec2(sidebar_w, sidebar_h));
            ImGui::Begin("Feeds");

            for (auto &feed : data.feeds_list)
            {
                if (!feed.erase)
                {
                    if (ImGui::Selectable(feed.title.c_str(),
                                (focus_feed == &feed)))
                    {
                        focus_feed = &feed;
                    }
                }
            }

            ImGui::End();
        }

        {   // Main RSS Content Window
            ImGui::SetNextWindowPos(ImVec2(sidebar_w, mainbar_size));
            ImGui::SetNextWindowSize(ImVec2(main_w, main_h));
            ImGui::Begin("RSS Feed");

            if (focus_feed != nullptr)
            {
                if (ImGui::Button("Update"))
                {
                    auto async_val = std::async(std::launch::async,
                            [&focus_feed]{
                                focus_feed->update();
                            });
                }

                ImGui::SameLine();

                if (ImGui::Button("Setting"))
                {

                }

                ImGui::SameLine();

                // TODO: Have a popup dialog first
                if (ImGui::Button("Remove"))
                {
                    focus_feed->erase = true;
                    focus_feed = nullptr;
                }
                else
                {
                    ImGui::Text(focus_feed->title.c_str());
                    ImGui::Text(focus_feed->link.c_str());
                    ImGui::Text(focus_feed->description.c_str());
                    ImGui::Text(focus_feed->language.c_str());

                    ImGui::Separator();

                    for (auto &[i_key, item] : focus_feed->items)
                    {
                        const std::string extraID = focus_feed->title + i_key;
                        if (ImGui::Button(("Read##" + extraID).c_str()))
                        {
                            focus_item = &item;
                            windows_feedItemContent = true;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button(("Open##" + extraID).c_str()))
                        {
                            rss::link::open(item.link, data.browser);
                        }
                        ImGui::SameLine();
                        ImGui::Text((item.pub_date_str() + " |").c_str());
                        ImGui::SameLine();
                        ImGui::TextWrapped(item.title.c_str());
                    }
                }
            }

            ImGui::End();
        }

        if (windows_newFeed)
        {   // New RSS Feed window
            {
                static char url[512];
                ImGui::Begin("New Feed");

                ImGui::InputText("url", url, 512);

                if (ImGui::Button("Create"))
                {
                    data.new_feed(url);
                    url[0] = '\0';
                    windows_newFeed = false;
                    focus_feed = nullptr;
                }

                if (ImGui::Button("Close"))
                {
                    url[0] = '\0';
                    windows_newFeed = false;
                }

                ImGui::End();
            }
        }

        if (windows_feedItemContent && focus_item != nullptr)
        {   // Feed item window
            {
                ImGui::Begin("Item");

                if (ImGui::Button("Close"))
                {
                    windows_feedItemContent = false;
                }

                ImGui::Text(focus_item->title.c_str());
                ImGui::Text(focus_item->link.c_str());
                ImGui::Text(focus_item->pub_date_str().c_str());

                ImGui::Separator();

                ImGui::TextWrapped(focus_item->description.c_str());

                ImGui::End();
            }
        }

        if (windows_settings)
        {   // Settings window
            {
                ImGui::Begin("Settings");

                if (ImGui::Button("Close"))
                {
                    windows_settings = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Save"))
                {
                    data.browser = browser;
                }

                ImGui::InputText("Browser", browser, 256);

                ImGui::End();
            }
        }

        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    data.save("test.json");
    return 0;
}

