/* $Id: oslibexcept.h,v 1.3 2003-01-05 12:31:24 sandervl Exp $ */
/*
 * Exception handler util. procedures
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __OSLIBEXCEPT_H__
#define __OSLIBEXCEPT_H__

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
//Dispatches OS/2 exception to win32 handler
//Returns: TRUE, win32 exception handler returned continue execution
//         FALSE, otherwise
//******************************************************************************
BOOL APIENTRY OSLibDispatchException(PEXCEPTIONREPORTRECORD pReportRec,
                                     PEXCEPTIONREGISTRATIONRECORD pRegistrationRec,
                                     PCONTEXTRECORD pContextRec, PVOID p);

#ifdef __cplusplus
}
#endif

#endif
