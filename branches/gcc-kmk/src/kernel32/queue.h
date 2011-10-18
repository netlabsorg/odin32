/* $Id: queue.h,v 1.1 2000-03-09 19:03:20 sandervl Exp $ */

/*
 * Very simply generic queue class
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

class Queue;

//******************************************************************************
//******************************************************************************
class QueueItem
{
public:
    QueueItem(ULONG dwItemData)
    {
        this->dwItemData = dwItemData;
        next = NULL;
    }

private:
    ULONG       dwItemData;
    QueueItem  *next;

    friend class Queue;
};
//******************************************************************************
//******************************************************************************
class Queue
{
public:
    Queue();
    ~Queue();

    void       Push(ULONG dwItemData);
    void       Remove(ULONG dwItemData);
    void       Remove(QueueItem *item);
    ULONG      Pop();

    void       operator=(Queue&) ;

    QueueItem *Head()                   { return head; };
    QueueItem *Tail()                   { return tail; };
    QueueItem *getNext(QueueItem *item) { return item->next; };
    ULONG      getItem(QueueItem *item) { return item->dwItemData; };

    BOOL       hasElements()            { return head != NULL; };

private:
    void       PushRev(Queue& queue, QueueItem *item);

    QueueItem *head;
    QueueItem *tail;
};
//******************************************************************************
//******************************************************************************

