project(regex)

set(regex-source-files "${PROJECT_SOURCE_DIR}/RegexTreeBuilder.cpp" ${nfa-source-files} CACHE INTERNAL "")
set(regex-libraries "nfa" CACHE INTERNAL "")

add_library(regex SHARED ${regex-source-files} ${formal_SOURCE_DIR}/include/regex/RegexTreeBuilder.h)
target_link_libraries(regex ${regex-libraries})