/* $Id: oslibexcept.h,v 1.2 2000-09-08 18:07:50 sandervl Exp $ */
/*
 * Exception handler util. procedures
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __OSLIBEXCEPT_H__
#define __OSLIBEXCEPT_H__

//******************************************************************************
//Dispatches OS/2 exception to win32 handler
//Returns: TRUE, win32 exception handler returned continue execution
//         FALSE, otherwise
//******************************************************************************
BOOL APIENTRY OSLibDispatchException(PEXCEPTIONREPORTRECORD pReportRec,
                                     PEXCEPTIONREGISTRATIONRECORD pRegistrationRec,
                                     PCONTEXTRECORD pContextRec, PVOID p);

#endif
