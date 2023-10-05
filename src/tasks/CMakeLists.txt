project(tasks)

add_executable(main main.cpp)
target_link_libraries(main nfa regex dfa)

add_executable(build_nfa_from_regex build_nfa_from_regex.cpp)
target_link_libraries(build_nfa_from_regex nfa regex)

add_executable(build_empty_edges_closure build_empty_edges_closure.cpp)
target_link_libraries(build_empty_edges_closure automaton nfa)

add_executable(build_dfa_from_nfa build_dfa_from_nfa.cpp)
target_link_libraries(build_dfa_from_nfa automaton nfa dfa)

add_executable(build_fdfa build_fdfa.cpp)
target_link_libraries(build_fdfa automaton dfa)

add_executable(build_complement_to_dfa build_complement_to_dfa.cpp)
target_link_libraries(build_complement_to_dfa automaton dfa)
