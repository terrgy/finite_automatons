project(tasks)

add_executable(main main.cpp)

add_executable(build_nfa_from_regex build_nfa_from_regex.cpp)
target_link_libraries(build_nfa_from_regex nfa regex)
