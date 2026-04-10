/**
 * @file test_runner.h
 * @brief Minimal test suite runner.
 *
 * Include this header once per test executable. It defines `tests_run`,
 * a TestCase struct, and run_suite() / finish_suite() helpers that replace
 * the per-file boilerplate previously duplicated across every test file.
 *
 * Usage:
 *   1. Include this header (it pulls in minunit.h automatically).
 *   2. Write test functions returning NULL on pass or an error string on fail.
 *   3. Declare a static TestCase array and call run_suite() + finish_suite()
 *      from main().
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "minunit.h"
#include <stdio.h>

int tests_run = 0;

/** A named pointer to a test function. */
typedef struct {
    const char *name;
    char *(*fn)(void);
} TestCase;

/**
 * @brief Run every test in @p tests, printing PASS/FAIL for each.
 *
 * Unlike the old mu_run_test macro, this does NOT stop at the first failure —
 * every test in the suite is always executed.
 *
 * @param suite_name Label printed as a header for this group of tests.
 * @param tests      Array of TestCase entries.
 * @param count      Number of entries in @p tests.
 * @return Number of failures (0 = all passed).
 */
static inline int run_suite(const char *suite_name, const TestCase *tests,
                            int count) {
    int failed = 0;
    printf("=== %s ===\n", suite_name);
    for (int i = 0; i < count; i++) {
        char *result = tests[i].fn();
        tests_run++;
        if (result) {
            printf("  FAIL [%s]: %s\n", tests[i].name, result);
            failed++;
        } else {
            printf("  PASS [%s]\n", tests[i].name);
        }
    }
    return failed;
}

/**
 * @brief Print a final summary and return an exit code.
 *
 * @param failed Number of failures returned by run_suite().
 * @return 0 if all tests passed, 1 otherwise (suitable for main() return).
 */
static inline int finish_suite(int failed) {
    printf("---\n");
    if (failed)
        printf("%d of %d tests FAILED\n", failed, tests_run);
    else
        printf("ALL %d TESTS PASSED\n", tests_run);
    return failed != 0;
}

#endif /* TEST_RUNNER_H */
