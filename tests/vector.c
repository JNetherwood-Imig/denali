#include "dwl/defines.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>

// TODO
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

[[maybe_unused]]static void print_vec(Vector(i32) vec) {
  vector_foreach(e, vec)
    printf("%i\n", e);
}

[[maybe_unused]]static void print_vec_r(Vector(i32) vec) {
  vector_foreach_r(e, vec)
    printf("%i\n", e);
}

int main(void) {
  Vector(i32) vec = vector_new(i32);
  assert(vector_size(vec) == 0 && vector_capacity(vec) == 1);
  assert(vector_empty(vec) == true);
  vector_destroy(vec);

  Vector(i32) vec2 = vector_reserve(i32, 16);
  vector_push_back(vec2, 1);
  assert(vector_empty(vec2) == false);
  vector_destroy(vec2);
}
