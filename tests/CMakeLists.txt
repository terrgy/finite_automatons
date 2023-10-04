project(all_tests)

include_directories(nfa)
include_directories(regex)
add_subdirectory(nfa)
add_subdirectory(regex)

add_executable(all_tests all_tests.cpp nfa/NFATestCase.cpp regex/RegexTestCase.cpp)
target_link_libraries(
        all_tests gtest_main gtest gmock Threads::Threads cpr curl nfa regex
)