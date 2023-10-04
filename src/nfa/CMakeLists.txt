project(nfa)

set(nfa-source-files "${PROJECT_SOURCE_DIR}/NFA.cpp" CACHE INTERNAL "")
set(nfa-libraries "" CACHE INTERNAL "")

add_library(nfa SHARED ${nfa-source-files} ${formal_SOURCE_DIR}/include/nfa/NFA.h)