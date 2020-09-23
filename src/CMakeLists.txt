add_executable(mfeed
    main.cpp
    test.cpp
    rss/downloader.cpp
    rss/extract.cpp
)

target_compile_features(mfeed PRIVATE cxx_std_20)
target_compile_options(mfeed PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)
target_link_libraries(mfeed
    PRIVATE
        fmt::fmt
        ${CURL_LIBRARIES}
        stdc++fs
        pugixml
)

