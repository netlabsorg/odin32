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
void (*SYSTRAY_ItemSetMessage)(SystrayItem *ptrayItem, ULONG uCallbackMessage);
void (*SYSTRAY_ItemSetIcon)(SystrayItem *ptrayItem, HICON hIcon);
void (*SYSTRAY_ItemSetTip)(SystrayItem *ptrayItem, CHAR* szTip, int modify);

#endif // __WINE_SYSTRAY_H
