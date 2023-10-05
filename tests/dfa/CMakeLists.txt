set(TARGET_NAME "dfa")
set(COVERAGE_NAME "dfa-coverage")
set(COVERAGE_TARGET "dfa-coverage-tree")
set(SOURCE_DIR_MAIN "${dfa_SOURCE_DIR}")
set(SOURCES ${dfa-source-files})
set(LIBRARIES ${dfa-libraries} regex)

message(INFO ${SOURCES})

set(TEST_SOURCES DFATestCase.h DFATestCase.cpp)
run_coverage(
        "${TEST_SOURCES}"
        "${SOURCES}"
        "${LIBRARIES}"
        "${COVERAGE_TARGET}"
        "${TARGET_NAME}"
        "${COVERAGE_NAME}"
)