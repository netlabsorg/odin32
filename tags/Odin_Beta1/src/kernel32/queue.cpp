/* $Id: queue.cpp,v 1.1 2000-03-09 19:03:20 sandervl Exp $ */

/*
 * Very simply generic queue class
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os2win.H>
#include <misc.h>
#include "queue.h"

#define DBG_LOCALLOG	DBG_queue
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
Queue::Queue()
{
    head = tail = NULL;
}
//******************************************************************************
//******************************************************************************
Queue::~Queue()
{
    while(hasElements()) {
        Pop();
    }
}
//******************************************************************************
//******************************************************************************
void Queue::operator=(Queue& queue) 
{
 QueueItem *item = queue.Head();

  if(item)
  	PushRev(queue, item);
}
//******************************************************************************
//******************************************************************************
void Queue::PushRev(Queue& queue, QueueItem *item)
{
 QueueItem *nextitem = queue.getNext(item);

  if(nextitem) {
	PushRev(queue, nextitem);
  }
  Push(queue.getItem(item));
}
//******************************************************************************
//******************************************************************************
void Queue::Push(ULONG dwItemData)
{
 QueueItem *item;

    item = new QueueItem(dwItemData);
    if(item == NULL) {
        DebugInt3();
        return;
    }
    if(head == NULL) {
        head = item;
        tail = item;
        item->next = NULL;
    }
    else {
        item->next = head;
        head = item;
    }
}
//******************************************************************************
//******************************************************************************
void Queue::Remove(ULONG dwItemData)
{
 QueueItem *item = head, *tmp;

    if(item == NULL) {
	DebugInt3();
	return;
    }
    if(item->dwItemData == dwItemData) {
	head = item->next;
	delete item;
	return;
    }
    while(item->next) {
    	if(item->next->dwItemData == dwItemData) {
		tmp = item->next;
		item->next = tmp->next;
		delete tmp;
		return;
	}
	item = item->next;
    }
    DebugInt3();
    dprintf(("Queue::Remove: item %x not found!", dwItemData));
}
//******************************************************************************
//******************************************************************************
void Queue::Remove(QueueItem *removeitem)
{
 QueueItem *item = head, *tmp;

    if(item == NULL) {
	DebugInt3();
	return;
    }
    if(item == removeitem) {
	head = item->next;
	delete item;
	return;
    }
    while(item->next) {
    	if(item->next == removeitem) {
		tmp = item->next;
		item->next = tmp->next;
		delete tmp;
		return;
	}
	item = item->next;
    }
    DebugInt3();
    dprintf(("Queue::Remove: item %x not found!", removeitem));
}
//******************************************************************************
//******************************************************************************
ULONG Queue::Pop()
{
 QueueItem *item;
 ULONG itemdata;

    if(head == NULL) {
	DebugInt3();
        return NULL;
    }
    item = head;
    head = item->next;
    if(head == NULL) {
        tail = NULL;
    }
    itemdata = item->dwItemData;
    delete item;
    return itemdata;
}
//******************************************************************************
//******************************************************************************

