/* minunit.h -- minimal unit testing macros (public domain) */
#ifndef MINUNIT_H
#define MINUNIT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern int tests_run;

#define mu_assert(message, test)                                               \
    do {                                                                       \
        if (!(test))                                                           \
            return message;                                                    \
    } while (0)
#define mu_run_test(test)                                                      \
    do {                                                                       \
        char *message = test();                                                \
        tests_run++;                                                           \
        if (message)                                                           \
            return message;                                                    \
    } while (0)

/* Floating-point approximate equality assertions */
#define mu_assert_double_eq(message, a, b, tol)                                \
    do {                                                                       \
        double _a = (double)(a);                                               \
        double _b = (double)(b);                                               \
        double _tol = (double)(tol);                                           \
        if (!(fabs(_a - _b) <= _tol))                                          \
            return message;                                                    \
    } while (0)

#define mu_assert_float_eq(message, a, b, tol)                                 \
    do {                                                                       \
        float _a = (float)(a);                                                 \
        float _b = (float)(b);                                                 \
        float _tol = (float)(tol);                                             \
        if (!(fabsf(_a - _b) <= _tol))                                         \
            return message;                                                    \
    } while (0)

#endif /* MINUNIT_H */
