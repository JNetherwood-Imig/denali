#include "list.h"

void list_init(List* list) { list->next = list->prev = nullptr; }

void list_insert(List* list, ListNode* node) {
  if (!list->next) {
    list->next = node;
    list->prev = node;
    node->next = list;
    node->prev = list;
  } else {
    node->next = list;
    node->prev = list->prev;
    list->prev->next = node;
    list->prev = node;
  }
}

void list_remove(ListNode* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}
