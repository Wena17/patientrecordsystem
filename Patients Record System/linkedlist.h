#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED

typedef struct s_linkedlist
{
    void *elem;
    struct s_linkedlist *next;
} LinkedList;

LinkedList *list_append(LinkedList *the_list, void *elem);
LinkedList *list_prepend(LinkedList *the_list, void *elem);
LinkedList *list_delete(LinkedList *list, void *elem);

#endif // LINKEDLIST_H_INCLUDED
