/* $Id: comboex.h,v 1.2 2000-02-23 17:03:00 cbratschi Exp $ */

/*
 * ComboBoxEx class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_COMBOEX_H
#define __WINE_COMBOEX_H

typedef struct tagCOMBOEX_INFO
{
    COMCTL32_HEADER header;
    HIMAGELIST      himl;
    HWND            hwndCombo;
    DWORD           dwExtStyle;

} COMBOEX_INFO;


extern VOID COMBOEX_Register (VOID);
extern VOID COMBOEX_Unregister (VOID);

#endif  /* __WINE_COMBOEX_H */
