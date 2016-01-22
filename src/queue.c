#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "printing.h"


Queue *queue_init()
{
    Queue *q = malloc(sizeof(Queue));
    q->head = malloc(sizeof(Node));
    q->tail = malloc(sizeof(Node));
    
    q->head->next = q->tail;
    q->tail->prev = q->head;
    return q;
}
void queue_fatal(Queue *q)
{
    Node *curr = q->head;
    Node *temp;
    while (curr->next != NULL)
    {
        temp = curr->next;
        free(curr->data);
        free(curr);
        curr = temp;
    }
}

void queue_enqueue(Queue *q, uchar *data, uint size)
{
    Node *new = malloc(sizeof(Node));
    q->head->next->prev = new;
    new->next = q->head->next;
    new->prev = q->head;
    q->head->next = new;

    new->data = malloc(size);
    new->size = size;
    memcpy(new->data, data, size);

    q->size++;
}
uint queue_dequeue(Queue *q, uchar *dest)
{
    if (q->size == 0)
        fatal("Dequeue attempted on empty queue");
    
    memcpy(dest, q->tail->prev->data, q->tail->prev->size);

    Node *temp = q->tail->prev;
    uint retval = temp->size;
    
    q->tail->prev->prev->next = q->tail;
    q->tail->prev = q->tail->prev->prev;

    free(temp->data);
    free(temp);

    q->size--;
    return retval;
}
