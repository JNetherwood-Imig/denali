#include "list.h"

void list_init(List* list) {
  list->next = list->prev = nullptr;
}

void list_insert(List* list, ListNode* node) {}

void list_remove(ListNode* node) {}
