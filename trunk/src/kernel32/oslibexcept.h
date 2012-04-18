/* $Id: oslibexcept.h,v 1.3 2003-01-05 12:31:24 sandervl Exp $ */
/*
 * Exception handler util. procedures
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __OSLIBEXCEPT_H__
#define __OSLIBEXCEPT_H__

BOOL APIENTRY OSLibConvertExceptionInfo(PEXCEPTIONREPORTRECORD pReportRec,
                                        PCONTEXTRECORD pContextRec,
                                        PWINEXCEPTION_RECORD pWinReportRec,
                                        PWINCONTEXT pWinContextRec,
                                        TEB *pWinTEB);

BOOL APIENTRY OSLibConvertExceptionResult(ULONG rc,
                                          PWINCONTEXT pWinContextRec,
                                          PCONTEXTRECORD pContextRec);

BOOL APIENTRY OSLibDispatchException(PEXCEPTIONREPORTRECORD pReportRec,
                                     PEXCEPTIONREGISTRATIONRECORD pRegistrationRec,
                                     PCONTEXTRECORD pContextRec, PVOID p,
                                     BOOL fSEH);

#endif
