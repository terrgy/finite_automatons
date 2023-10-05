project(nfa)

set(nfa-source-files "${PROJECT_SOURCE_DIR}/NFA.cpp" ${automaton-source-files} CACHE INTERNAL "")
set(nfa-libraries "automaton" CACHE INTERNAL "")

add_library(nfa SHARED ${PROJECT_SOURCE_DIR}/NFA.cpp ${formal_SOURCE_DIR}/include/nfa/NFA.h)
target_link_libraries(nfa ${nfa-libraries})