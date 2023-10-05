project(all_tests)

include_directories(nfa)
include_directories(regex)
include_directories(dfa)
add_subdirectory(nfa)
add_subdirectory(regex)
add_subdirectory(dfa)

add_executable(all_tests all_tests.cpp
        nfa/NFATestCase.cpp regex/RegexTestCase.cpp dfa/DFATestCase.cpp
        ${automaton-source-files}
        ${nfa-source-files}
        ${regex-source-files}
        ${dfa-source-files}
)
target_link_libraries(
        all_tests gtest_main gtest gmock Threads::Threads cpr curl
)

setup_target_for_coverage(
        all_tests_coverage_tree "." all_tests all_tests_coverage
)

add_dependencies(coverage_report all_tests_coverage_tree)