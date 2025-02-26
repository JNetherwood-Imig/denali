#pragma once

// Vector(T) -> T*
// vector_new(T) -> Vector(T),
// vector_reserve(T, cap) -> Vector(T),
// vector_destroy(self) -> void
// vector_size(self) -> size,
// vector_capacity(self) -> capacity,
// vector_grow(self) -> void,
// vector_shrink_to_fit(self) -> void,
// vector_front(self) -> first,
// vector_back(self) -> last,
// vector_begin(self) -> &first,
// vector_end(self) -> &last
// vector_insert(self, elem, idx) -> void,
// vector_insert_range(self, ptr, count, idx) -> void,
// vector_push_front(self, elem) -> void,
// vector_push_range_front(self, ptr, count) -> void,
// vector_push_back(self, elem) -> void,
// vector_push_range_back(self, ptr, count) -> void,
// vector_pop(self, idx) -> elem,
// vector_pop_front(self) -> first,
// vector_pop_back(self) -> last,
// vector_erase(self, idx) -> void
// vector_erase_front(self) -> void
// vector_erase_back(self) -> void
// vector_erase_first_match(self, elem) -> index
// vector_erase_all_matches(self, elem) -> void
// vector_clear(self) -> void,
// vector_empty(self) -> len == 0
// vector_foreach(elem, self) {...}
// vector_foreach_r(elem, self) {...}

#include "dwl/defines.h"

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

typedef struct __VecHead {
    usize size;
    usize capacity;
    void (*pfn_destroy)(void* elem);
} __VecHead;

#define __VEC_HEAD_SIZE sizeof(__VecHead)

#define __VEC_GROWTH_FACTOR 1.3

#define __vec_head(self) (((__VecHead*)(self)) - 1)

#define __vec_from_head(head) (void*)((head) + 1)

#define __vec_elem_size(self) sizeof((self)[0])

#define __vec_create(T, _capacity, _pfn_destroy)                                    \
    ({                                                                              \
        __VecHead* head = __vec_alloc(__VEC_HEAD_SIZE + (_capacity) * sizeof(T));   \
        head->size = 0;                                                             \
        head->capacity = (_capacity);                                               \
        head->pfn_destroy = (_pfn_destroy);                                         \
        __vec_from_head(head);                                                      \
    })

#define Vector(T) T*

#define vector_new(T) __vec_create(T, 1, nullptr)

#define vector_new_with_destructor(T, pfn_destroy) __vec_create(T, 1, pfn_destroy)

#define vector_reserve(T, capacity) __vec_create(T, capacity, nullptr)

#define vector_reserve_with_destructor(T, capacity, pfn_destroy)                    \
    __vec_create(T, capacity, pfn_destroy)

#define vector_destroy(self)                                                        \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot destroy null vector");            \
        __vec_free(__vec_head(self));                                               \
    } while (false)

#define vector_size(self) __vec_head(self)->size

#define vector_capacity(self) __vec_head(self)->capacity

#define vector_grow(self)                                                           \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot grow null vector");               \
        __VecHead* head = __vec_head(self);                                         \
        const usize new_capacity = (head->capacity + 1) * __VEC_GROWTH_FACTOR;      \
        __VecHead* new =                                                            \
            realloc(head, __VEC_HEAD_SIZE + new_capacity * __vec_elem_size(self));  \
        __vec_assert(new != nullptr && "failed to grow vector allocation");         \
        new->capacity = new_capacity;                                               \
        (self) = (typeof(self))__vec_from_head(new);                                \
    } while (false)

#define vector_shrink_to_fit(self)                                                  \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot shrink null vector");             \
        __VecHead* head = __vec_head(self);                                         \
        if (head->size == head->capacity)                                           \
            break;                                                                  \
        __VecHead* new =                                                            \
            realloc(head, __VEC_HEAD_SIZE + head->size * __vec_elem_size(self));    \
        __vec_assert(new != nullptr && "failed to shrink vector allocation");       \
        new->capacity = new->size;                                                  \
        (self) = (typeof(self))__vec_from_head(new);                                \
    } while (false)

#define vector_front(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot access front element of null vector");                 \
        (self)[0];                                                                  \
    })

#define vector_back(self)                                                           \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot access back element of null vector");                  \
        (self)[vector_size(self) - 1];                                              \
    })

#define vector_begin(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot get begin iterator for null vector");                  \
        (self);                                                                     \
    })

#define vector_end(self)                                                            \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot get end iterator for null vector");                    \
        (self) + vector_size(self);                                                 \
    })

#define vector_insert(self, element, index)                                         \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot insert into null vector");        \
        __vec_assert((index) >= 0 && (usize)(index) < vector_size(self) &&          \
                     "insert index out-of-bounds");                                 \
        if (vector_size(self) + 1 > vector_capacity(self))                          \
            vector_grow(self);                                                      \
        __vec_memmove((self) + (index) + 1, (self) + (index),                       \
                      (vector_size(self) - (index)) * __vec_elem_size(self));       \
        (self)[index] = (element);                                                  \
        ++vector_size(self);                                                        \
    } while (false)

#define vector_insert_range(self, elements, count, index)                           \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot insert range into null vector");  \
        __vec_assert((index) >= 0 && (index) < vector_size(self) &&                 \
                     "insert_range index out-of-bounds");                           \
        while (vector_size(self) + (count) > vector_capacity(self))                 \
            vector_grow(self);                                                      \
        __vec_memmove((self) + (index) + (count), (self) + (index),                 \
                      (vector_size(self) - (index) - 1) * __vec_elem_size(self);    \
        __vec_memcpy((self) + (index), (elements), (count) * __vec_elem_size(self); \
        vector_size(self) += (count);                                               \
    } while (false)

#define vector_push_front(self, element)\
    vector_insert(self, element, 0)

#define vector_push_range_front(self, elements, count)                              \
    vector_insert_range(self, elements, count, 0)

#define vector_push_back(self, element)                                             \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot push back to null vector");       \
        if (vector_size(self) + 1 > vector_capacity(self))                          \
            vector_grow(self);                                                      \
        (self)[vector_size(self)++] = (element);                                    \
    } while (false)

#define vector_push_range_back(self, elements, count)                               \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot push range back to null vector"); \
        while (vector_size(self) + (count) > vector_capacity(self))                 \
            vector_grow(self);                                                      \
        __vec_memcpy(vector_back(self), (elements),                                 \
                     (count) * __vec_elem_size(self));                              \
        vector_size(self) += (count);                                               \
    } while (false)

#define vector_pop(self, index)                                                     \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot pop element from null vector");   \
        __vec_assert((index) >= 0 && (index) < vector_size(self) &&                 \
                     "pop index out-of-bounds");                                    \
        typeof((self)[0]) elem = (self)[index];                                     \
        __vec_memmove((self) + (index), (self) + (index) + 1,                       \
                      (vector_size(self)-- - (index) - 1) * __vec_elem_size(self)); \
        elem;                                                                       \
    })

#define vector_pop_front(self)\
    vector_pop(self, 0)

#define vector_pop_back(self)                                                       \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot pop element from null vector");   \
        if (vector_size(self) < 1)                                                  \
            break;                                                                  \
        (self)[--vector_size(self)];                                                \
    })

#define vector_erase(self, index)                                                   \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot erase element from null vector"); \
        __vec_assert((isize)(index) >= 0 && (index) < (isize)vector_size(self) &&   \
                     "erase index out-of-bounds");                                  \
        __VecHead* head = __vec_head(self);                                         \
        if (head->pfn_destroy)                                                      \
            head->pfn_destroy((self) + (index));                                    \
        __vec_memmove((self) + (index), (self) + (index) + 1,                       \
                      (head->size-- - (index) - 1) * __vec_elem_size(self));        \
    } while (false)

#define vector_erase_front(self)\
    vector_erase(self, 0)

#define vector_erase_back(self)                                                     \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot erase element from null vector"); \
        if (vector_size(self) < 1)                                                  \
            break;                                                                  \
        __VecHead* head = __vec_head(self);                                         \
        if (head->pfn_destroy)                                                      \
            head->pfn_destroy((self) + --head->size);                               \
    } while (false)

#define vector_erase_first_match(self, element)                                     \
    ({                                                                              \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot erase_first_match from null vector");                  \
        isize match = -1;                                                           \
        do {                                                                        \
            for (isize i = 0; i < (isize)vector_size(self); ++i) {                  \
                if ((self)[i] == (element)) {                                       \
                    vector_erase(self, i);                                          \
                    match = i;                                                      \
                    break;                                                          \
                }                                                                   \
            };                                                                      \
        } while (false);                                                            \
        match;                                                                      \
    })

#define vector_erase_all_matches(self, element)                                     \
    do {                                                                            \
        __vec_assert((self) != nullptr &&                                           \
                     "cannot erase_all_matches from null vector");                  \
        for (isize i = 0; i < (isize)vector_size(self); ++i) {                      \
            if ((self)[i] == (element)) {                                           \
                vector_erase(self, i);                                              \
                --i;                                                                \
            }                                                                       \
        }                                                                           \
    } while (false)

#define vector_clear(self)                                                          \
    do {                                                                            \
        __vec_assert((self) != nullptr && "cannot clear null vector");              \
        if (vector_size(self) < 1)                                                  \
            break;                                                                  \
        vector_erase_back(self);                                                    \
    } while (vector_length(self) > 0)

#define vector_empty(self)                                                          \
    ({                                                                              \
        __vec_assert((self) != nullptr && "cannot check null vector for empty");    \
        vector_size(self) == 0;                                                     \
    })

#define vector_foreach(it_name, self)                                               \
    for (typeof((self)[0]) it_name = vector_front(self),                            \
                           *__vec_it = vector_begin(self);                          \
         __vec_it != vector_end(self); ++__vec_it, it_name = *__vec_it)

#define vector_foreach_r(it_name, self)                                             \
    for (typeof((self)[0]) it_name = vector_back(self),                             \
                           *__vec_it = vector_end(self) - 1;                        \
         __vec_it != vector_begin(self) - 1; --__vec_it, it_name = *__vec_it)
