set(TARGET_NAME "regex")
set(COVERAGE_NAME "regex-coverage")
set(COVERAGE_TARGET "regex-coverage-tree")
set(SOURCE_DIR_MAIN "${regex_SOURCE_DIR}")
set(SOURCES "${regex-source-files}")
set(LIBRARIES "${regex-libraries}")

message(INFO ${SOURCES})

set(TEST_SOURCES RegexTestCase.h RegexTestCase.cpp)
run_coverage(
        "${TEST_SOURCES}"
        "${SOURCES}"
        "${LIBRARIES}"
        "${COVERAGE_TARGET}"
        "${TARGET_NAME}"
        "${COVERAGE_NAME}"
)