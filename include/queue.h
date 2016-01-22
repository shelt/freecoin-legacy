#ifndef QUEUE_H
#define QUEUE_H

#include "shared.h"

typedef struct Node
{
    struct Node *prev;
    struct Node *next;
    uint size;
    uchar *data;
} Node;
typedef struct
{
    Node *head;
    Node *tail;
    uint size;
} Queue;


Queue *queue_init();
void queue_fatal(Queue *q);
void queue_enqueue(Queue *q, uchar *data, uint size);

uint queue_dequeue(Queue *q, uchar *dest);


#endif
