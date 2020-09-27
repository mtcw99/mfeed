#include <string>
#include <filesystem>
#include <iostream>
#include <sstream>

#include "date/date.h"
#include <fmt/format.h>

#include "rss/data.hpp"
#include "rss/feed.hpp"

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, PROG_NAME,
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
    int display_w, display_h;
    rss::feed *focus_feed = nullptr;

    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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

                if (ImGui::MenuItem("Quit"))
                {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
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
                if (ImGui::Button(feed.title.c_str(), ImVec2(sidebar_w - 20, 20)))
                {
                    focus_feed = &feed;
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
                ImGui::Text(focus_feed->title.c_str());
                ImGui::Text(focus_feed->link.c_str());
                ImGui::Text(focus_feed->description.c_str());
                ImGui::Text(focus_feed->language.c_str());

                ImGui::Separator();

                for (auto &item : focus_feed->items)
                {
                    ImGui::Text(item.title.c_str());
                }
            }

            ImGui::End();
        }

        if (windows_newFeed)
        {   // New RSS Feed window
            {
                static char name[512];
                static char url[512];
                ImGui::Begin("New Feed");

                ImGui::InputText("name", name, 512);
                ImGui::InputText("url", url, 512);

                if (ImGui::Button("Create"))
                {
                    fmt::print("New feed: name: {} url: {}\n",
                            name, url);
                }

                if (ImGui::Button("Close"))
                {
                    name[0] = '\0';
                    url[0] = '\0';
                    windows_newFeed = false;
                }

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

#if 0
    data.new_feed("https://www.phoronix.com/rss.php", "phoronix.rss");

    rss::feed &feed = data.feeds_list[0];

    fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
            feed.title, feed.link, feed.description, feed.language);

    for (auto &item : feed.items)
    {
        std::stringstream os_pubdate;
        os_pubdate << date::format("%F %T", item.pub_date);
        fmt::print("\nTitle: {}\nDate: {}\nLink: {}\nDescription: {}\n",
                item.title, os_pubdate.str(), item.link, item.description);
    }

    data.save("test.json");
    data.load("test.json");

    for (auto &feed : data.feeds_list)
    {
        fmt::print("Title: {}\nLink: {}\nDesc: {}\nLang: {}\n",
                feed.title, feed.link, feed.description, feed.language);

        for (auto &item : feed.items)
        {
            fmt::print("\nTitle: {}\nDate: {}\nLink: {}\nDescription: {}\n",
                    item.title, item.pub_date_str(), item.link, item.description);
        }
    }

    std::cin.get();
#endif

    return 0;
}

