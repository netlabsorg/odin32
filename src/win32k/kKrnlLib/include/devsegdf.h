/* $Id: devsegdf.h,v 1.2 2001-09-16 03:11:33 bird Exp $
 *
 * Segment stuff used in 16 and 32 bits C/C++.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _devSegDf_h
#define _devSegDf_h

#if defined(INCL_16) && !defined(EXTRACT)

/*
 * More segment stuff
 */
#define DATA16_INIT     _based(_segname("DATA16_INIT"))
#define DATA16_GLOBAL   _based(_segname("DATA16_GLOBAL"))
#define DATA16          _based(_segname("DATA16"))

#else

/*
 * More segment stuff
 */
#define DATA16_INIT
#define DATA16_GLOBAL
#define DATA16

#endif


#endif


