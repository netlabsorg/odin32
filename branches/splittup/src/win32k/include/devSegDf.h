/* $Id: devSegDf.h,v 1.2.2.1 2002-04-01 10:01:02 bird Exp $
 *
 * Segment stuff used in 16 and 32 bits C/C++.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _devSegDf_h
#define _devSegDf_h

#ifdef INCL_16

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


