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

#include "gui/main_ui.hpp"

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
    data.load_fb("data.fbb");

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
    const float FONT_SIZE = 20.0;
    const float SCALE_SIZE = 1.8;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.NavActive = true;
    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;
    fontConfig.OversampleH = 1;
    fontConfig.OversampleV = 1;
    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/noto/NotoSans-Regular.ttf",
            FONT_SIZE);
    const char *notosanscjk = "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc";
#if 0
    if (data.load_cjk)
    {
        io.Fonts->AddFontFromFileTTF(notosanscjk,
                FONT_SIZE, &fontConfig, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->AddFontFromFileTTF(notosanscjk,
                FONT_SIZE, &fontConfig, io.Fonts->GetGlyphRangesJapanese());
        io.Fonts->AddFontFromFileTTF(notosanscjk,
                FONT_SIZE, &fontConfig, io.Fonts->GetGlyphRangesKorean());
        io.Fonts->Build();
    }
#endif
    ImVector<ImWchar> imRanges;
    ImFontGlyphRangesBuilder imRangesBuilder;

    for (const auto &feed : data.feeds_list)
    {
        if ((feed.type == rss::e_feed_type::atom) || (
                    feed.type == rss::e_feed_type::rss && (
                        feed.language.starts_with("zh")
                        )))
        {
#if 0
            fmt::print("({}) desc: {} | {}\n", feed.type_str(), feed.description,
                    feed.type);
#endif
            imRangesBuilder.AddText(feed.title.c_str());
            imRangesBuilder.AddText(feed.description.c_str());
            for (const auto &[_, item] : feed.items)
            {
                imRangesBuilder.AddText(item.title.c_str());
                for (const auto &line : item.content)
                {
                    imRangesBuilder.AddText(line.c_str());
                }
            }
        }
    }

    imRangesBuilder.BuildRanges(&imRanges);
    io.Fonts->AddFontFromFileTTF(notosanscjk,
            FONT_SIZE, &fontConfig, imRanges.Data);
    io.Fonts->Build();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(SCALE_SIZE);
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    gui::main_ui main(SCALE_SIZE, data, window);
    uint32_t frame_count = 0;
    double time_previous = glfwGetTime();

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
            main.msf = static_cast<double>(1000.0 / double(frame_count));
            main.fps = static_cast<uint32_t>(1000.0 / main.msf);
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

        main.windows_loop();

        ImGui::Render();
        glfwGetFramebufferSize(window, &main.display_w, &main.display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (main.save_on_exit)
    {
        data.save_fb("data.fbb");
    }
    return 0;
}

