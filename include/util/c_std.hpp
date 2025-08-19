#ifndef _UTIL_C_STD_H
#define _UTIL_C_STD_H

#include "sds/sds.h"
#include "stb_ds/stb_ds.h"
#include <inttypes.h>
// TODO
#include <new>
#include <stdbool.h>
#include <stddef.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// C std

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String

typedef sds string_t;
#define str_new(X) X ? sdsnew(X) : NULL
#define str_delete(X)      \
    if (X) {               \
        sdsfree(X);        \
        X = str_new(NULL); \
    }
#define str_move(X, Y)      \
    do {                    \
        str_delete(*Y);     \
        *Y = *X;            \
        *X = str_new(NULL); \
    }                       \
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
#define vec_new() NULL
#define vec_delete(X)  \
    if (X) {           \
        arrfree(X);    \
        X = vec_new(); \
    }
#define vec_move(X, Y)  \
    do {                \
        vec_delete(*Y); \
        *Y = *X;        \
        *X = vec_new(); \
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

#define pair_t(TK, TV) Pair##TK##TV
#define PairKeyValue(TK, TV)        \
    typedef struct pair_t(TK, TV) { \
        TK key;                     \
        TV value;                   \
    }                               \
    pair_t(TK, TV)
#define pair_first(X) (X).key
#define pair_second(X) (X).value

#define hashmap_t(TK, TV) pair_t(TK, TV)*
#define map_new() NULL
#define map_delete(X)  \
    if (X) {           \
        hmfree(X);     \
        X = map_new(); \
    }
#define map_move(X, Y)  \
    do {                \
        map_delete(*Y); \
        *Y = *X;        \
        *X = map_new(); \
    }                   \
    while (0)
#define map_size(X) hmlenu(X)
#define map_add(X, Y, Z) hmput(X, Y, Z)
#define map_clear(X) map_delete(X)
#define map_empty(X) (map_size(X) == 0)
#define map_end() -1
#define map_erase(X, Y) hmdel(X, Y)
#define map_find(X, Y) hmgeti(X, Y)
#define map_get(X, Y) hmget(X, Y)
// TODO
#define map_move_add(X, Y, Z)                                                                                      \
    do {                                                                                                           \
        (X) = stbds_hmput_key_wrapper((X), sizeof *(X), (void*)STBDS_ADDRESSOF((X)->key, (Y)), sizeof(X)->key, 0); \
        (X)[stbds_temp((X)-1)].key = (Y);                                                                          \
        new (&(X)[stbds_temp((X)-1)].value) auto(std::move(Z));                                                    \
    }                                                                                                              \
    while (0)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hashset

#define element_t(TK) Element##TK
#define ElementKey(TK)             \
    typedef struct element_t(TK) { \
        TK key;                    \
        int8_t value;              \
    }                              \
    element_t(TK)
#define element_get(X) (X).key

#define hashset_t(TK) element_t(TK)*
#define set_new() map_new()
#define set_delete(X) map_delete(X)
#define set_size(X) map_size(X)
#define set_clear(X) map_clear(X)
#define set_end() map_end()
#define set_find(X, Y) map_find(X, Y)
#define set_insert(X, Y) map_add(X, Y, 0)

#endif
