add_subdirectory(utils)
add_subdirectory(string_search_algorithms)

add_library(StringFinder
        StringFinder.h StringFinder.cpp
        ExternStringFinder.h ExternStringFinder.cpp
)
target_link_libraries(StringFinder PUBLIC pthread Timer FileChunk TSQueue Task ESFMetaFile ZstdWrapper)