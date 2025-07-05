#ifndef _UTIL_C_STD_H
#define _UTIL_C_STD_H

#include "sds/sds.h"
#include "stb_ds/stb_ds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// C std

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String

#define string_t sds
#define str_delete(X) \
    if (X) {          \
        sdsfree(X);   \
        X = NULL;     \
    }
#define str_new(X) X ? sdsnew(X) : NULL
#define str_move(X, Y) \
    if (!*Y) {         \
        *Y = *X;       \
        *X = NULL;     \
    }
#define str_size(X) sdslen(X)
#define str_append(X, Y)  \
    do {                  \
        X = sdscat(X, Y); \
    }                     \
    while (0)
#define str_copy(X, Y) \
    do {               \
        str_delete(Y); \
        Y = sdsdup(X); \
    }                  \
    while (0)
#define str_to_string(X) sdsfromlonglong((long long)(X))
// #define str_substr_view(X, Y, Z) sdsrange(X, Y, Z)
#define str_substr(X, Y, Z, S) \
    do {                       \
        str_copy(X, S);        \
        sdsrange(S, Y, Z);     \
    }                          \
    while (0)
// #define str_reserve
#define str_resize(X, Y)       \
    do {                       \
        X = sdsgrowzero(X, Y); \
    }                          \
    while (0)
#define str_clear(X) sdsclear(X)
#define str_back(X) (X)[str_size(X) - 1]

#endif
