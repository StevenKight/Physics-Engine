/* minunit.h -- minimal unit testing macros (public domain) */
#ifndef MINUNIT_H
#define MINUNIT_H

#include <math.h>

extern int tests_run;

#define mu_assert(message, test)                                               \
    do {                                                                       \
        if (!(test))                                                           \
            return message;                                                    \
    } while (0)

#define mu_assert_double_eq(message, a, b, tol)                               \
    do {                                                                       \
        double _a = (double)(a);                                               \
        double _b = (double)(b);                                               \
        double _tol = (double)(tol);                                           \
        if (!(fabs(_a - _b) <= _tol))                                          \
            return message;                                                    \
    } while (0)

#endif /* MINUNIT_H */
