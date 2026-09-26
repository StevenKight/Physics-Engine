/* minunit.h -- minimal unit testing macros (public domain) */
#ifndef MINUNIT_H
#define MINUNIT_H

#include <math.h>

extern int tests_run;

/* Sentinel returned by a test function to mark it skipped rather than
 * passed/failed (e.g. a GPU test with no CUDA device present). Compared
 * by pointer identity against this dedicated object, not string content
 * (comparing == against a string literal is unspecified behavior). */
char mu_skip_marker;
#define MU_SKIP (&mu_skip_marker)

#define mu_assert(message, test)                                               \
    do {                                                                       \
        if (!(test))                                                           \
            return message;                                                    \
    } while (0)

#define mu_assert_double_eq(message, a, b, tol)                                \
    do {                                                                       \
        double _a = (double)(a);                                               \
        double _b = (double)(b);                                               \
        double _tol = (double)(tol);                                           \
        if (!(fabs(_a - _b) <= _tol))                                          \
            return message;                                                    \
    } while (0)

#endif /* MINUNIT_H */
