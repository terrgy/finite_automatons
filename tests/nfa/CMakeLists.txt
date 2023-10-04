set(TARGET_NAME "nfa")
set(COVERAGE_NAME "nfa-coverage")
set(COVERAGE_TARGET "nfa-coverage-tree")
set(SOURCE_DIR_MAIN "${nfa_SOURCE_DIR}")
set(SOURCES ${nfa-source-files})
set(LIBRARIES ${nfa-libraries})

message(INFO ${SOURCES})

set(TEST_SOURCES NFATestCase.h NFATestCase.cpp)
run_coverage(
        "${TEST_SOURCES}"
        "${SOURCES}"
        "${LIBRARIES}"
        "${COVERAGE_TARGET}"
        "${TARGET_NAME}"
        "${COVERAGE_NAME}"
)

set_property(TARGET nfa-test PROPERTY ENABLE_EXPORTS 1)