#include <stddef.h>
#include <stdlib.h>

#include "linkedlist.h"

/* This is implementing queue behavior. */
LinkedList *list_append(LinkedList *list, void *elem)
{
    LinkedList *new_link = malloc(sizeof(LinkedList));
    new_link->elem = elem;
    new_link->next = NULL;

    if (list)
    {
        LinkedList *last = list;
        while (last->next)
            last = last->next;
        last->next = new_link;
        return list;
    }
    else
    {
        return new_link;
    }

}

/* This is implementing stack behavior. */
LinkedList *list_prepend(LinkedList *the_list, void *elem) {
    LinkedList *new_link = malloc(sizeof(LinkedList));
    new_link->elem = elem;
    new_link->next = the_list;
    return new_link;
}

LinkedList *list_delete(LinkedList *list, void *elem)
{
    LinkedList *previous = NULL;
    LinkedList *current = list;
    while (current)
    {
        if (current->elem == elem)
        {
            if (previous)
            {
                previous->next = current->next;
                free(current);
                return list;
            }
            else
            {
                LinkedList *result = current->next;
                free(current);
                return result;
            }
        }
        previous = current;
        current = current->next;
    }
    return list;
}
