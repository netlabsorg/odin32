/* $Id: kKLkernel.h,v 1.1 2002-04-01 13:43:45 bird Exp $
 *
 *
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _kKLkernel_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* fKernel */
#define KF_UNI              0x0000
#define KF_SMP              0x0001
#define KF_W4               0x0002
#define KF_REV_MASK         0x0FF0
#define KF_REV_SHIFT        4
#define KF_REV_0            0x0000
#define KF_REV_A            0x0010
#define KF_REV_B            0x0020
#define KF_REV_C            0x0030
#define KF_REV_D            0x0040
#define KF_REV_E            0x0050
#define KF_REV_F            0x0060
#define KF_REV_G            0x0070
#define KF_REV_H            0x0080
#define KF_REV_I            0x0090
#define KF_REV_J            0x00a0
#define KF_REV_K            0x00b0
#define KF_REV_L            0x00c0
#define KF_REV_M            0x00d0
#define KF_REV_N            0x00e0
#define KF_REV_O            0x00f0
#define KF_REV_P            0x0100
#define KF_REV_Q            0x0110
#define KF_REV_R            0x0120
#define KF_REV_S            0x0130
#define KF_REV_T            0x0140
#define KF_REV_U            0x0150
#define KF_REV_V            0x0160
#define KF_REV_W            0x0170
#define KF_REV_X            0x0180
#define KF_REV_Y            0x0190
#define KF_REV_Z            0x01a0
#define KF_REV_ECS          0x0900
#define KF_DEBUG            0x1000
#define KF_HAS_DEBUGTYPE    0x2000
#define KF_ALLSTRICT        0x3000
#define KF_HALFSTRICT       0x7000


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern ULONG    fKernel;
extern ULONG    ulKernelBuild;
#if defined(KKRNLLIB) && defined(RING0) && defined(INCL_32)
    #if defined(__IBMC__) || defined(__IBMCPP__)
        #pragma map(fKernel,        "_fKernel")
        #pragma map(ulKernelBuild,  "_ulKernelBuild")
    #endif
#endif

#endif
