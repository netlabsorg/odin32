/*
 *  Systray.
 *
 *  Common definitions.
 */

#ifndef __WINE_SYSTRAY_H
#define __WINE_SYSTRAY_H

struct _SystrayItem;

typedef struct _SystrayItem SystrayItem;

BOOL (*SYSTRAY_ItemInit)(SystrayItem *ptrayItem);
void (*SYSTRAY_ItemTerm)(SystrayItem *ptrayItem);
void (*SYSTRAY_ItemUpdate)(SystrayItem *ptrayItem, ULONG uFlags);

SystrayItem *SYSTRAY_FindItem(ULONG uIdx);
void SYSTRAY_PruneAllItems(void);

#endif // __WINE_SYSTRAY_H
