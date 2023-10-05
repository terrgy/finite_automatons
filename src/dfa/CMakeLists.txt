project(dfa)

set(dfa-source-files "${PROJECT_SOURCE_DIR}/DFA.cpp" CACHE INTERNAL "")
set(dfa-libraries automaton nfa CACHE INTERNAL "")

add_library(dfa SHARED ${dfa-source-files} ${formal_SOURCE_DIR}/include/dfa/DFA.h)
target_link_libraries(dfa ${dfa-libraries})