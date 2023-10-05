project(automaton)

set(automaton-source-files "${PROJECT_SOURCE_DIR}/Automaton.cpp" CACHE INTERNAL "")
set(automaton-libraries "" CACHE INTERNAL "")

add_library(automaton SHARED ${automaton-source-files} ${formal_SOURCE_DIR}/include/automaton/Automaton.h)