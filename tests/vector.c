#include "vector.h"
#include "dwl/defines.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// [X] Vector(T) -> T*
// [X] vector_new(T) -> Vector(T),
// [X] vector_new_with_destructor(T, pfn_destroy) -> Vector(T)
// [X] vector_reserve(T, cap) -> Vector(T),
// [X] vector_reserve_with_destructor(T, cap, pfn_destroy) -> Vector(T)
// [X] vector_destroy(self) -> void
// [X] vector_size(self) -> size,
// [X] vector_capacity(self) -> capacity,
// [X] vector_grow(self) -> void,
// [X] vector_shrink_to_fit(self) -> void,
// [X] vector_get(self, index) -> elem
// [X] vector_set(self, index, value) -> void
// [X] vector_front(self) -> first,
// [X] vector_back(self) -> last,
// [X] vector_begin(self) -> &first,
// [X] vector_end(self) -> &(last + 1)
// [X] vector_rbegin(self) -> &last
// [X] vector_rend(self) -> &(first - 1)
// [X] vector_insert(self, elem, idx) -> void,
// [X] vector_insert_range(self, ptr, count, idx) -> void,
// [X] vector_push_front(self, elem) -> void,
// [X] vector_push_range_front(self, ptr, count) -> void,
// [X] vector_push_back(self, elem) -> void,
// [X] vector_push_range_back(self, ptr, count) -> void,
// [X] vector_pop(self, idx) -> elem,
// [X] vector_pop_front(self) -> first,
// [X] vector_pop_back(self) -> last,
// [X] vector_erase(self, idx) -> void
// [X] vector_erase_front(self) -> void
// [X] vector_erase_back(self) -> void
// [X] vector_erase_first_match(self, elem) -> index
// [X] vector_erase_all_matches(self, elem) -> void
// [X] vector_clear(self) -> void,
// [X] vector_empty(self) -> len == 0
// [X] vector_foreach(elem, self) {...}
// [X] vector_foreach_r(elem, self) {...}

typedef struct Test {
    i32 val;
} Test;

static Test* test_new(i32 val) {
    Test* test = malloc(sizeof(Test));
    test->val = val;
    return test;
}

static void test_destroy(Test* test) { free(test); }

static void test_vec_destructor(void* elem_ptr) { test_destroy(*(Test**)elem_ptr); }

int main(void) {
    // Creates new vector of i32s
    Vector(i32) vec = vector_new(i32);
    // Pushes 1-10 to back of vector
    for (u32 i = 1; i <= 10; ++i) {
        vector_push_back(vec, i);
    }

    // Inserts a 0 in between each element
    for (u32 i = 0; i < vector_size(vec); i += 2) {
        vector_insert(vec, i, 0);
    }

    // Removes last element and returns it
    [[maybe_unused]] i32 elem = vector_pop_back(vec);

    // Removes and returns first element
    elem = vector_pop_front(vec);

    // Removes and returns vec[4]
    elem = vector_pop(vec, 4);

    // Erases all 0s from vector, calls destructor if available (not in this case)
    vector_erase_all_matches(vec, 0);
    // Shrinks vector allocation to match size
    vector_shrink_to_fit(vec);

    // Erase first 7 from vector and returns index
    [[maybe_unused]] usize index = vector_erase_first_match(vec, 7);

    i32 range[] = {1, 2, 3, 4};
    // Inserts the range at index 3
    vector_insert_range(vec, 3, range, 4);
    // Appends the range to back
    vector_push_range_back(vec, range, 4);

    // Cleanly and safely iterates over each element in the vector
    vector_foreach(elem, vec) { printf("%i\n", elem); }

    // Reverse iterator
    vector_foreach_r(elem, vec) { printf("%i\n", elem); }

    // Clears vector
    vector_clear(vec);
    // Vector now reports as empty
    assert(vector_empty(vec) == true);

    // Frees vector
    vector_destroy(vec);

    // Creates a vector with enough storage for 16 Test*, and sets the destructor
    Vector(Test*) vec2 =
        vector_reserve_with_destructor(Test*, 16, test_vec_destructor);

    // Pushes newly allocated test to vector
    vector_push_back(vec2, test_new(0));

    // Removes last element and DOES NOT return it
    // Because of this, it will check if it has a destructor and call it
    // with a POINTER to the element (i.e. a Test**)
    // vector_pop does not call the destructor because it returns the element
    vector_erase_back(vec2);

    // Uncomment to force failure due to bounds checking
    // vector_get(vec2, 1);
    // vector_get(vec2, -1);
    // vector_set(vec2, 1, nullptr);

    // Free vector
    vector_destroy(vec2);
}
