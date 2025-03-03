#pragma once

#include "dwl/defines.h"

#include <stddef.h>

typedef struct ListNode ListNode;
typedef ListNode List;
struct ListNode {
    ListNode* prev;
    ListNode* next;
};

void list_init(List* list);

void list_insert(List* list, ListNode* node);

void list_append(List* list, ListNode* node);

void list_remove(ListNode* node);

#define list_foreach(container, list, link_field)                                   \
    for ((container) =                                                              \
             field_parent_ptr((list)->next, typeof(*container), link_field);        \
         &(container)->link_field != (list);                                        \
         (container) = field_parent_ptr((container)->link_field.next,               \
                                        typeof(*container), link_field))

#define list_foreach_s(container, tmp, list, link_field)                            \
    for ((container) =                                                              \
             field_parent_ptr((list)->next, typeof(*container), link_field),        \
        (tmp) = field_parent_ptr((container)->link_field.next, typeof(*container),  \
                                 link_field);                                       \
         &(container)->link_field != (list); (container) = (tmp),                   \
        (tmp) = field_parent_ptr((tmp)->link_field.next, typeof(*container),        \
                                 link_field))

#define list_foreach_r(container, list, link_field)                                 \
    for ((container) =                                                              \
             field_parent_ptr((list)->prev, typeof(*container), link_field);        \
         &(container)->link_field != (list);                                        \
         (container) = field_parent_ptr((container)->link_field.prev,               \
                                        typeof(*container), link_field))

#define list_foreach_r_s(container, tmp, list, link_field)                          \
    for ((container) =                                                              \
             field_parent_ptr((list)->prev, typeof(*container), link_field),        \
        (tmp) = field_parent_ptr((container)->link_field.prev, typeof(*container),  \
                                 link_field);                                       \
         &(container)->link_field != (list); (container) = (tmp),                   \
        (tmp) = field_parent_ptr((tmp)->link_field.prev, typeof(*container),        \
                                 link_field))
