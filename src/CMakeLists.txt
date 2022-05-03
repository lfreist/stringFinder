add_subdirectory(utils)
add_subdirectory(string_search_algorithms)

add_library(ExternStringFinder
        ExternStringFinder.h ExternStringFinder.cpp
)
target_link_libraries(ExternStringFinder PUBLIC pthread Timer TSQueue Buffer ESFMetaFile)

add_library(StringFinder
        StringFinder.h StringFinder.cpp
        InteractiveStringFinder.h InteractiveStringFinder.cpp
)
target_link_libraries(StringFinder PUBLIC OpenMP::OpenMP_CXX Timer)