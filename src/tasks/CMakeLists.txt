project(tasks)

add_executable(main main.cpp)
target_link_libraries(main nfa regex)

add_executable(build_nfa_from_regex build_nfa_from_regex.cpp)
target_link_libraries(build_nfa_from_regex nfa regex)

add_executable(build_empty_edges_closure build_empty_edges_closure.cpp)
target_link_libraries(build_empty_edges_closure nfa)
