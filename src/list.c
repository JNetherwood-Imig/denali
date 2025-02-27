#include "list.h"

void list_init(List* list) { list->next = list->prev = list; }

void list_append(List* list, ListNode* node) {
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}

void list_insert(List* list, ListNode* node) {
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}

void list_remove(ListNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
