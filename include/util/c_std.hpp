#ifndef _UTIL_C_STD_H
#define _UTIL_C_STD_H

#include "sds/sds.h"
#include "stb_ds/stb_ds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// C std

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String

typedef sds string_t;
#define str_delete(X) \
    if (X) {          \
        sdsfree(X);   \
        X = NULL;     \
    }
#define str_new(X) X ? sdsnew(X) : NULL
#define str_move(X, Y)  \
    do {                \
        str_delete(*Y); \
        *Y = *X;        \
        *X = NULL;      \
    }                   \
    while (0)
#define str_size(X) sdslen(X)
#define str_back(X) (X)[str_size(X) - 1]
#define str_append(X, Y)  \
    do {                  \
        X = sdscat(X, Y); \
    }                     \
    while (0)
#define str_clear(X) sdsclear(X)
#define str_copy(X, Y) \
    do {               \
        str_delete(Y); \
        Y = sdsdup(X); \
    }                  \
    while (0)
#define str_hash(X) stbds_hash_string(X, 42)
#define str_pop_back(X) sdsrange(X, 0, -2)
#define str_push_back(X, Y)             \
    do {                                \
        str_resize(X, str_size(X) + 1); \
        str_back(X) = Y;                \
    }                                   \
    while (0)
#define str_reserve(X, Y)         \
    do {                          \
        X = sdsMakeRoomFor(X, Y); \
    }                             \
    while (0)
#define str_resize(X, Y)       \
    do {                       \
        X = sdsgrowzero(X, Y); \
    }                          \
    while (0)
#define str_substr(X, Y, Z) sdsrange(X, Y, Z)
#define str_to_string(X) (X) > 0 ? sdsfromunsignedlonglong((unsigned long long)(X)) : sdsfromlonglong((long long)(X))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Vector

#define vector_t(T) T*
#define vec_delete(X) \
    if (X) {          \
        arrfree(X);   \
        X = NULL;     \
    }
#define vec_new() NULL
#define vec_move(X, Y)  \
    do {                \
        vec_delete(*Y); \
        *Y = *X;        \
        *X = NULL;      \
    }                   \
    while (0)
#define vec_size(X) arrlenu(X)
#define vec_back(X) (X)[vec_size(X) - 1]
#define vec_clear(X)                 \
    if (X) {                         \
        stbds_header(X)->length = 0; \
    }
#define vec_empty(X) (vec_size(X) == 0)
// TODO
#include <new>
#define vec_move_back(X, Y)                                       \
    do {                                                          \
        stbds_arrmaybegrow(X, 1);                                 \
        new (&(X)[stbds_header(X)->length++]) auto(std::move(Y)); \
    }                                                             \
    while (0)
#define vec_pop_back(X) arrpop(X)
#define vec_push_back(X, Y) arrput(X, Y)
#define vec_remove_swap(X, Y) arrdelswap(X, Y)
#define vec_resize(X, Y) arrsetlen(X, Y)
#define vec_reserve(X, Y) arrsetcap(X, Y)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hashmap

#define PairKeyValue(T1, T2)      \
    typedef struct Pair##T1##T2 { \
        T1 key;                   \
        T2 value;                 \
    } Pair##T1##T2;

#define hashmap_t(T1, T2) Pair##T1##T2*
#define map_delete(X) \
    if (X) {          \
        hmfree(X);    \
        X = NULL;     \
    }
#define map_new() NULL
#define map_move(X, Y)  \
    do {                \
        map_delete(*Y); \
        *Y = *X;        \
        *X = NULL;      \
    }                   \
    while (0)
#define map_size(X) hmlenu(X)

#endif
