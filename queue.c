#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head;
    queue_contex_t *contex = (queue_contex_t *) malloc(sizeof(queue_contex_t));
    head = &contex->chain;
    INIT_LIST_HEAD(head);
    contex->size = 0;

    return head;
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
    contex->size++;
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    new_node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!new_node->value) {
        free(new_node);
        return false;
    }
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
    contex->size++;
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;
    new_node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    strncpy(new_node->value, s, (strlen(s) + 1));  // plus one for '\0'
    list_add_tail(&new_node->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    if (list_empty(head))
        return NULL;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    contex->size--;
    element_t *removed_node = container_of(head->next, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, removed_node->value, bufsize - 1);
        removed_node->value[bufsize - 1] = '\0';
    }

    list_del_init(head->next);
    return removed_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    if (list_empty(head))
        return NULL;

    queue_contex_t *contex = container_of(head, queue_contex_t, chain);
    contex->size--;
    element_t *removed_node = container_of(head->prev, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, removed_node->value, bufsize - 1);
        removed_node->value[bufsize - 1] = '\0';
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
    int pos = (contex->size) / 2;
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
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
