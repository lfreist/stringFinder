add_subdirectory(utils)
add_subdirectory(string_search_algorithms)

add_library(ExternStringFinder
        ExternStringFinder.h ExternStringFinder.cpp
        utils/output/tree.h
        utils/math.h
)
target_link_libraries(ExternStringFinder PUBLIC pthread Timer TSQueue FileChunk ESFMetaFile)

add_library(StringFinder
        InternStringFinder.h InternStringFinder.cpp
        InteractiveStringFinder.h InteractiveStringFinder.cpp
)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX Timer)