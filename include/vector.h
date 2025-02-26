#pragma once

#include "dwl/defines.h"

#define __VEC_GROWTH_FACTOR 1.3

#ifndef __vec_alloc
#include <stdlib.h>
#define __vec_alloc malloc
#endif

#ifndef __vec_realloc
#include <stdlib.h>
#define __vec_realloc realloc
#endif

#ifndef __vec_free
#include <stdlib.h>
#define __vec_free free
#endif

#ifndef __vec_memmove
#include <string.h>
#define __vec_memmove memmove
#endif

#ifndef __vec_memcpy
#include <string.h>
#define __vec_memcpy memcpy
#endif

#ifndef __vec_assert
#include <assert.h>
#define __vec_assert assert
#endif

typedef void (*PfnElemDestructor)(void*);

typedef struct VectorHeader {
    usize size;
    usize capacity;
    PfnElemDestructor pfn_destroy;
} VectorHeader;

#define __vec_head(self) (((VectorHeader*)(self)) - 1)
#define __vec_from_head(head) ((head) + 1)
#define __vec_create(T, capacity, pfn_destroy)                                      \
    ({                                                                              \
        VectorHeader* head =                                                        \
            __vec_alloc(sizeof(VectorHeader) + (capacity) * sizeof(T));             \
        head->size = 0;                                                             \
        head->capacity = (capacity);                                                \
        head->pfn_destroy = (pfn_destroy);                                            \
        (Vector(T))__vec_from_head(head);                                           \
    })

// Type definition
// Vector(T) -> T*
#define Vector(T) T*

// Allocation functions
// vector_new(T) -> Vector(T),
#define vector_new(T) __vec_create(T, 1, nullptr)
#define vector_new_with_destructor(T, pfn_destroy) __vec_create(T, 1, pfn_destroy)
// vector_reserve(T, cap) -> Vector(T),
#define vector_reserve(T, capacity) __vec_create(T, capacity, nullptr)
#define vector_reserve_with_destructor(T, capacity, pfn_destroy)                    \
    __vec_create(T, capacity, pfn_destroy)
// vector_destroy(self) -> void
#define vector_destroy(self)                                                        \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot destroy null vector");            \
        __vec_free(__vec_head(self))                                                \
    } while (false)
// vector_size(self) -> size,
#define vector_size(self)                                                           \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot get size of null vector");        \
        __vec_head(self)->size;                                                     \
    })
// vector_capacity(self) -> capacity,
#define vector_capacity(self)                                                       \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot get capacity of null vector");    \
        __vec_head(self)->capacity;                                                 \
    })
// vector_grow(self) -> void,
#define vector_grow(self)                                                           \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot grow null vector");               \
        VectorHeader* head = __vec_head(self);                                      \
        const usize new_capacity = (head->capacity + 1) * __VEC_GROWTH_FACTOR;      \
        VectorHeader* new =                                                         \
            realloc(head, sizeof(VectorHeader) + new_capacity * sizeof((self)[0])); \
        __vec_assert(new != nullptr && "failed to grow vector allocation");         \
        new->capacity = new_capacity;                                               \
        (self) = (typeof(self))__vec_from_head(new);                                \
    } while (false)
// vector_shrink_to_fit(self) -> void,
#define vector_shrink_to_fit(self)                                                  \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot shrink null vector");             \
        VectorHeader* head = __vec_head(self);                                      \
        if (head->size == head->capacity)                                           \
            break;                                                                  \
        VectorHeader* new =                                                         \
            realloc(head, sizeof(VectorHeader) + head->size * sizeof((self)[0]));   \
        __vec_assert(new != nullptr && "failed to shrink vector allocation");       \
        new->capacity = new->size;                                                  \
        (self) = (typeof(self))__vec_from_head(new);                                \
    } while (false)

// Check if empty
// vector_empty(self) -> len == 0
#define vector_empty(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot check null vector for empty");    \
        vector_size(self) == 0;                                                     \
    })

// Access utils
// vector_front(self) -> first,
#define vector_front(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot access front element of null vector");                 \
        (self)[0];                                                                  \
    })
// vector_back(self) -> last,
#define vector_back(self)                                                           \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot access back element of null vector");                  \
        (self)[vector_size(self) - 1];                                              \
    })
// vector_begin(self) -> &first,
#define vector_begin(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot get begin iterator for null vector");                  \
        (self);                                                                     \
    })
// vector_end(self) -> &last
#define vector_end(self)                                                            \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot get end iterator for null vector");                    \
        (self) + (vector_size(self) * sizeof((self)[0]));                           \
    })

// Data manipulation
// vector_insert(self, elem, idx) -> void,
#define vector_insert(self, element, index)                                         \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot insert into null vector");        \
        if (vector_size(self) + 1 > vector_capacity(self))                          \
            vector_grow(self);                                                      \
        __vec_memmove((self) + (index) + 1, (self) + (index),                       \
                      (vector_size(self) - (index) - 1) * sizeof((self)[0]));       \
        (self)[index] = (element);                                                  \
        ++vector_size(self);\
    } while (false)
// vector_insert_range(self, ptr, count, idx) -> void,
#define vector_insert_range(self, elements, count, index)                           \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot insert range into null vector");  \
        while (vector_size(self) + (count) > vector_capacity(self))                 \
            vector_grow(self);                                                      \
        __vec_memmove((self) + (index) + (count), (self) + (index),                 \
                      (vector_size(self) - (index) - 1) * sizeof((self)[0]));       \
        __vec_memcpy((self) + (index), (elements), (count) * sizeof((self)[0]));    \
        vector_size(self) += (count);\
    } while (false)
// vector_push_back(self, elem) -> void,
#define vector_push_back(self, element)\
    do {\
        __vec_assert((self) != nullptr && "cannot push back to null vector");\
        if (vector_size(self) + 1 > vector_capacity(self)) vector_grow(self);\
        (self)[vector_size(self)++] = (element);\
    } while (false)
// vector_push_range_back(self, ptr, count) -> void,
#define vector_push_range_back(self, elements, count)\
    do {\
        __vec_assert((self) != nullptr && "cannot push range back to null vector");\
        while (vector_size(self) + (count) > vector_capacity(self)) vector_grow(self);\
        __vec_memcpy(vector_back(self), (elements), (count) * sizeof((self)[0]));\
        vector_size(self) += (count);\
    } while (false)
// vector_push_front(self, elem) -> void,
#define vector_push_front(self, element) vector_insert(self, element, 0)
// vector_push_range_front(self, ptr, count) -> void,
#define vector_push_range_front(self, elements, count) vector_insert_range(self, elements, count, 0)
// vector_pop(self, idx) -> elem,
#define vector_pop(self, index)\
    ({\
        typeof((self)[0]) elem = (self)[index];\
        __vec_memmove((self) + (index), (self) + (index) + 1, (vector_size(self)-- - (index) - 2) * sizeof((self)[0]));\
        elem;\
    })
// vector_pop_back(self) -> last,
#define vector_pop_back(self)\
    ({})
// vector_pop_front(self) -> first,
// vector_clear(self) -> void,
// vector_erase(self, idx) -> void
// vector_erase_back(self) -> void
// vector_erase_front(self) -> void
