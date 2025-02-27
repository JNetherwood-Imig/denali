#include "list.h"
#include "dwl/defines.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Test {
    i32 val;
    ListNode link;
} Test;

int main(void) {
    List list;
    list_init(&list);

    Test* t1 = malloc(sizeof(Test));
    t1->val = 1;
    list_insert(&list, &t1->link);

    Test* t2 = malloc(sizeof(Test));
    t2->val = 2;
    list_append(&list, &t2->link);

    Test* t;
    Test* tmp;
    list_foreach_r_s(t, tmp, &list, link) {
        printf("%i\n", t->val);
        list_remove(&t->link);
        free(t);
    }
}
