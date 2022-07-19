add_subdirectory(utils)
add_subdirectory(string_search_algorithms)

add_library(StringFinder
        StringFinder.h StringFinder.cpp
        )
target_link_libraries(StringFinder PUBLIC pthread Timer FileChunk TSQueue Task SFMetaFile ZstdWrapper)

add_library(ESF
        ExternStringFinder.h ExternStringFinder.cpp
        )
target_link_libraries(ESF PUBLIC StringFinder SFReaders)

add_library(ISF
        InternStringFinder.h InternStringFinder.cpp
        )
target_link_libraries(ISF PUBLIC StringFinder SFReaders)