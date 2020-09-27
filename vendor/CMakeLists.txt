get_filename_component(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

list(APPEND IMGUI_PUBLIC_HEADERS
    imgui/imgui.h
    imgui/imgui_impl_glfw.h
)

list(APPEND IMGUI_PRIVATE_HEADERS
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_truetype.h
    imgui/imstb_textedit.h
)

list(APPEND IMGUI_SOURCES
    imgui/imgui_demo.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
)

list(APPEND IMGUI_PUBLIC_INCLUDE_DIRS
    imgui/
)

list(APPEND IMGUI_PUBLIC_LIBRARIES)

add_library(glad glad/glad.h glad/glad.c)
target_include_directories(glad
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_link_libraries(imgui PUBLIC ${IMGUI_PUBLIC_LIBRARIES})
target_include_directories(imgui
    PUBLIC ${IMGUI_PUBLIC_INCLUDE_DIRS}
    PRIVATE ${IMGUI_PRIVATE_INCLUDE_DIRS}
)

