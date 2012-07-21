/* $Id: input.h,v 1.1 1999-05-24 20:19:13 ktk Exp $ */

/*
 * USER input header file
 * Copyright 1997 David Faure
 *
 */

#ifndef __WINE_INPUT_H
#define __WINE_INPUT_H

extern BOOL MouseButtonsStates[3];
extern BOOL AsyncMouseButtonsStates[3];
extern BYTE InputKeyStateTable[256];
extern BYTE QueueKeyStateTable[256];
extern BYTE AsyncKeyStateTable[256];

#endif  /* __WINE_INPUT_H */

