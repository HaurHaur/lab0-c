#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right);
struct list_head *merge_sort(struct list_head *head, int size);

/* Create an empty queue */
struct list_head *q_new()
{
    queue_contex_t *contex = (queue_contex_t *) malloc(sizeof(queue_contex_t));
    if (!contex)
        return NULL;
    INIT_LIST_HEAD(&contex->chain);
    contex->size = 0;

    return &contex->chain;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *cur, *tmp;
    list_for_each_entry_safe (cur, tmp, l, list) {
        q_release_element(cur);
    }
    queue_contex_t *contex = container_of(l, queue_contex_t, chain);
    free(contex);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    new_node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    contex->size++;
    strncpy(new_node->value, s, (strlen(s) + 1));  // plus one for '\0'
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    new_node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    contex->size++;
    strncpy(new_node->value, s, (strlen(s) + 1));  // plus one for '\0'
    list_add_tail(&new_node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    contex->size--;
    element_t *removed_node = container_of(head->next, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, removed_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);
    return removed_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    contex->size--;
    element_t *removed_node = container_of(head->prev, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, removed_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->prev);

    return removed_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    return contex->size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    int pos = (contex->size) / 2;  // position of element that should be delete
    contex->size--;
    element_t *cur, *tmp;
    list_for_each_entry_safe (cur, tmp, head, list) {
        if (pos == 0) {
            list_del(&cur->list);
            q_release_element(cur);
            return true;
        }
        pos--;
    }
    return false;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    bool f = false;
    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        if (!f && safe != head) {
            if (strcmp(list_entry(cur, element_t, list)->value,
                       list_entry(safe, element_t, list)->value) == 0) {
                list_del(cur);
                q_release_element(list_entry(cur, element_t, list));
                contex->size--;
                f = true;
            }
        }

        else if (f) {
            f = safe != head
                    ? strcmp(list_entry(cur, element_t, list)->value,
                             list_entry(safe, element_t, list)->value) == 0
                    : false;
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
            contex->size--;
        }
    }
    return true;
}

/* Swap two list_heads of queue and move two pointers to next position*/
void list_swap(struct list_head **cur, struct list_head **next)
{
    if (!cur || !next) {
        return;
    }

    (*cur)->next = (*next)->next;
    (*next)->prev = (*cur)->prev;
    (*cur)->next->prev = *cur;
    (*next)->prev->next = *next;
    (*cur)->prev = *next;
    (*next)->next = *cur;
    *next = (*cur)->next;
    return;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur, *next;
    list_for_each_safe (cur, next, head) {
        if (next == head)
            return;
        list_swap(&cur, &next);
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur, *tmp;
    list_for_each_safe (cur, tmp, head) {
        cur->next = cur->prev;
        cur->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    int size = q_size(head), c = 0;
    struct list_head *cur, *safe, *first = head;
    list_for_each_safe (cur, safe, head) {
        if (size - c >= k) {
            if (c % k == 0) {
                first = cur->prev;
                cur->prev = cur->next;
            }

            if (c % k == k - 1) {
                first->next->next = cur->next;
                cur->next->prev = first->next;
                cur->next = cur->prev;
                first->next = cur;
                cur->prev = first;
            }

            if (c % k != 0 && c % k != k - 1) {
                cur->next = cur->prev;
                cur->prev = safe;
            }
        }
        c++;
    }
    return;
}

// merge sort with header
// struct list_head *mergeTwoLists(struct list_head *left, struct list_head
// *right)
// {
//     if (!left || !right)
//         return right;
//     struct list_head **node = NULL, *head = right, **ptr = &head->next, *cur,
//                      *safe;
//     right = right->next;
//     for (; left != head && right != head; *node = (*node)->next) {
//         node = strcmp(container_of(left, element_t, list)->value,
//                       container_of(right, element_t, list)->value) <= 0
//                    ? &left
//                    : &right;
//         *ptr = *node;
//         ptr = &(*ptr)->next;
//     }
//     *ptr = left == head ? right : left;
//     list_for_each_safe (cur, safe, head) {  // recover prev link
//         safe->prev = cur;
//     }
//     head->next->prev = head;

//     return head;
// }

/* Sort elements of queue in ascending order */
// void q_sort(struct list_head *head)
// {
//     if (!head || list_empty(head))
//         return;

//     if (list_is_singular(head)) {
//         head->next->prev = head;
//         return;
//     }

//     struct list_head *cur, *safe, *left, *right, *last;
//     queue_contex_t *contex = container_of(head, queue_contex_t, chain);
//     int n = contex->size;
//     int pos = n / 2;
//     list_for_each_safe (cur, safe, head) {  // devide
//         if (pos == 1) {
//             last = head->prev;
//             head->prev = cur;
//             cur->next = head;
//             contex->size = n / 2;
//             q_sort(head);
//             left = head->next;
//             head->next = safe;
//             head->prev = last;
//             contex->size = n - n / 2;
//             q_sort(head);
//             right = head;
//             contex->size = n;
//             mergeTwoLists(left, right);
//             safe = head;
//         }
//         pos--;
//     }
//     return;
// }

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur, *safe;
    head->prev = NULL;
    head->next = merge_sort(head->next, q_size(head));
    for (cur = head, safe = cur->next; safe; cur = safe, safe = safe->next) {
        safe->prev = cur;
    }
    cur->next = head;
    head->prev = cur;
    return;
}

struct list_head *merge_sort(struct list_head *head, int size)
{
    if (size == 1) {
        head->next = NULL;
        return head;
    }

    struct list_head *right = NULL, *left = head;
    int pos = size / 2;
    while (pos != 1) {
        left = left->next;
        pos--;
    }
    right = left->next;
    left->next = NULL;
    left = merge_sort(head, size / 2);
    right = merge_sort(right, size - size / 2);
    head = merge_two_list(left, right);
    return head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    struct list_head *cur, *safe;
    char *max_c = container_of(head->prev, element_t, list)->value;
    element_t *e;
    for (cur = head->prev->prev, safe = cur->prev; cur != head;
         cur = safe, safe = safe->prev) {
        e = container_of(cur, element_t, list);
        if (strcmp(e->value, max_c) < 0) {
            list_del(cur);
            q_release_element(e);
            contex->size--;
        } else {
            max_c = e->value;
        }
    }
    return contex->size;
}

// /* Merge all the queues into one sorted queue, which is in ascending order */
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right)
{
    if (!left || !right)
        return !left ? right : left;
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; left && right; ptr = &(*ptr)->next) {
        node = strcmp(container_of(left, element_t, list)->value,
                      container_of(right, element_t, list)->value) <= 0
                   ? &left
                   : &right;
        *ptr = *node;
        *node = (*node)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) left | (uintptr_t) right);
    return head;
}

int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    queue_contex_t *cur, *safe;
    struct list_head *left, *right,
        *rm = NULL, **ptr = &rm,
        *q_head = container_of(head->next, queue_contex_t, chain)->q;
    queue_contex_t *contex = container_of(q_head, queue_contex_t, chain);
    while (head->next != head->prev) {
        list_for_each_entry_safe (cur, safe, head, chain) {
            if (&safe->chain != head) {
                cur->q->prev->next = NULL;
                safe->q->prev->next = NULL;
                cur->q->next =
                    merge_two_list(list_empty(cur->q) ? NULL : cur->q->next,
                                   list_empty(safe->q) ? NULL : safe->q->next);
                cur = safe;
                safe =
                    container_of((&(safe->chain))->next, queue_contex_t, chain);
                list_del_init(&cur->chain);
                INIT_LIST_HEAD(cur->q);
                *ptr = &(cur->chain);
                ptr = &(*ptr)->next;
            }
        }
    }
    *ptr = NULL;
    for (left = rm, right = left->next; left;
         left = right, right = right ? right->next : NULL) {
        list_add_tail(left, head);
    }

    size_t s = 0;
    for (left = q_head, right = left->next; right != NULL;
         left = right, right = right->next) {  // recover prev link
        right->prev = left;
        s++;
    }
    left->next = q_head;
    q_head->prev = left;
    contex->size = s;
    return s;
}

// merge with first queue
// int q_merge(struct list_head *head)
// {
//     if (!head || list_empty(head))
//         return 0;
//     if (list_is_singular(head))
//         return 1;
//     queue_contex_t *cur, *first = list_first_entry(head, queue_contex_t,
//     chain); first->q->prev->next = NULL; struct list_head *left, *right;
//     list_for_each_entry (cur, head, chain) {
//         if (cur != first) {
//             cur->q->prev->next = NULL;
//             // list_entry(first->q, queue_contex_t, chain)->size
//             first->q->next =
//                 merge_two_list(list_empty(cur->q) ? NULL : cur->q->next,
//                                 list_empty(first->q) ? NULL :
//                                 first->q->next);
//             INIT_LIST_HEAD(cur->q);
//         }

//     }
//     size_t s = 0;
//     for (left = first->q, right = left->next; right != NULL;
//          left = right, right = right->next) {  // recover prev link
//         right->prev = left;
//         s++;
//     }
//     left->next = first->q;
//     first->q->prev = left;
//     first->size = s;
//     return s;
// }
