file(GLOB_RECURSE SOURCES "*.cpp")
add_library(blocktree STATIC ${SOURCES})
target_link_libraries(blocktree sdsl divsufsort divsufsort64)

