/*****************************************************************************
 *
 * SOURCE FILE NAME = SVGADEFS.H
 *
 * DESCRIPTIVE NAME =
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1991, 1992
 *             Copyright Microsoft Corporation, 1990
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *             RESTRICTED MATERIALS OF IBM
 *             IBM CONFIDENTIAL
 *
 * VERSION = V2.0
 *
 * DATE
 *
 * DESCRIPTION  Contains SVGA definitions.
 *
 * FUNCTIONS
 *
 * NOTES        When included into VVID sources, #ifdef SVGA
 *              must be used before #include!
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *   DATE      FLAG        APAR   CHANGE DESCRIPTION
 *   --------  ----------  -----  --------------------------------------
 *   mm/dd/yy  @Vr.mpppxx   xxxxx xxxxxxx
 *   12/29/92  @V2.0SEN00         Created.
 *   03/12/93  @V2.0SEN01         OEMINFO structure changed.
 *   08/31/93  @V2.1YEE02         Recognize Cirrus Logic GD5428 chip
 *   10/28/93  @V2.1MNH00         Recognize Additional Adapters
 *   12/06/93  @V2.1YEE05 D 76476 Add ID for Chips & Technologies
 *   12/15/93  @V2.1YEE06 D 76685 Add Artist Graphics manufacturer
 *   02/21/94  @V2.1MNH01 D 79562 Recognize Additional Adapters/Chips
 *   04/14/94  @V2.1MNH05 D 81883 WD90C24 allow more unlocks
 *   04/26/94  @V2.1MNH08 D 82003 WD90C24 preserve locks for BIOS setmodes
 *   05/12/94  @V2.1YEE08         Add more Cirrus chipsets
 *   05/13/94  @V2.1MNH13 F 74819 ATI Mach8/32 check in files
 *   05/18/94  @V2.1MNH14         WD90C33 Parse new style PMI
 *   06/22/94  @V2.1JWK02 D 87347 Define Tseng Chip Names for consistant use
 *   07/25/94  @V2.1JWK03 D 89911 Define Tseng W32xD Revisions
 *   07/28/94  @V2.1YEE09 D 88172 Define a Chips and Technologies chiptype
 *   08/02/94  @V2.1MNH25 D 91551 Fix various DAC programming problems
 *   08/19/94  @V2.1MNH37 D 94209 Add more DAC detection
 *   09/01/94  @V2.1YEE10 D 89853 Add support for S3864 with ICD2061 clock chip
 *   09/10/94  @V2.2TSU00 D 86373 Weitek: PCI screen01 incorrect
 *   09/15/94  @V2.1YEE11         Identify DAC as RGB (blue first) or BGR (red first)
 *   09/17/94  @V2.2DAI01 D 92593 Adding monitor configuration enhancements.
 *   09/26/94  @V2.2SEN05 D 97637 Changing Music DAC 4910 from BGR to RGB.
 *   10/05/94  @V2.1MNH51 D101610 Increase size of CLEANDATA for MACH32
 *   10/25/94  @V2.2JWK23 D103192 TSENG Rev D wont install
 *   11/30/94  @V3.0YEE01 D105950 Support S3 964
 *   01/20/95  @V3.0YEE02 D 99391 Support TBird-Enhanced (800x600 flat panel)
 *   01/16/95  @V3.0YEE03 D114992 Add new chip manufacturers
 *   01/24/95  @V3.0ITO01         ThinkPad 230 Support (IBM-J local)
 *   04/19/95  @V3.0DAI01 D119247 Add DevEscape functions and structures for extra capability support
 *   04/28/95  @V3.0YEE04 D120303 Treat ICS5342 unique from ICS5341
 *   05/17/95  @V3.0JWK04 D113889 Add Toshiba manufacturer and models
 *   05/30/95  @V3.0JWK11 D123835 allow displayless machines to init.  Requires
 *                                /nodisplay on device driver statement in config.sys
 *   07/25/95  @IBMJ-S3GEN   D386 More S3 chips support
 *   07/27/95  @V3.0JWK18 D131213 fix AT&T 408/409/499 DAC programming
 *   08/02/95  @V3.0JWK21 D131829 Identify and Support ATI MACH64 version 'CT'
 *   09/11/95  @V3.2SEN01 D133791 Adding real mode software interrupt support to
 *                                VIDEOPMI.
 *   09/14/95  @V3.0JWK25 D126118 NEC C24 prevent 800x600x64k.  Add NEC Manufacturer and model
 *   11/10/95  @V3.2JWK09 D143505 provide compatability with new VIDEO_ADAPTER structures
 *   12/15/95  @V3.0TSU00         Make videopmi backward compatible
 *   02/12/96  @V3.1MNH01 D148203 Merge DBCS-J source w/SBCS source
 *   02/14/96  @V3.2TSU00 D148283 Merlin PBE, support linear aperture
 *   02/16/96  @V3.0YEE06 D148329 APM support
 *   05/14/96  @V3.1MNH10 D151513 S3 Trio Chips Identification
 *****************************************************************************/

/* NOINC */

#include <svgapmi.h>

/* INC */

#define INCL_DOSERRORS
#define MAX_LOCKUNLOCKCMD       40                              /*@V2.1MNH08*/
#define MAX_CLEANDATA          200      /* Includes DACs */     /*@V3.0YEE01*/
#define MAX_SETBANK_CMD         25                              /*@V2.1MNH14*/
#define MAX_GETBANK_CMD         20                              /*@V2.1MNH14*/
#define MAX_MODESET_CMD         84                              /*@V2.1MNH14*/


#define DEFAULT_ADAPTER       (-1)
#define UNKNOWN_ADAPTER         0
#define VIDEO7_ADAPTER          1       /* bugbug line up with cleanups! */
#define TRIDENT_ADAPTER         2
#define TSENG_ADAPTER           3
#define WESTERNDIG_ADAPTER      4
#define ATI_ADAPTER             5
#define IBM_ADAPTER             6
#define CIRRUS_ADAPTER          7
#define S3_ADAPTER              8
#define CHIPS_ADAPTER           9                      /* @V2.1YEE05 */
#define WEITEK_ADAPTER          10                     /* @V2.1MNH00 */
#define NUMBER9_ADAPTER         11                     /* @V2.1YEE10 */
#define GENERIC_PCISVGA_ADAPTER 12                      //@senja
#define OAK_ADAPTER             13                     /* @V3.0YEE03 */
#define MATROX_ADAPTER          14                     /* @V3.0YEE03 */
#define BROOKTREE_ADAPTER       15                     /* @V3.0YEE03 */
#define NVIDIA_ADAPTER          16                     /* @V3.0YEE03 */
#define ALLIANCE_ADAPTER        17                     /* @V3.0YEE03 */
#define AVANCE_ADAPTER          18                     /* @V3.0YEE03 */
#define MEDIAVISION_ADAPTER     19                     /* @V3.0YEE03 */
#define ARKLOGIC_ADAPTER        20                     /* @V3.0YEE03 */
#define RADIUS_ADAPTER          21                     /* @V3.0YEE03 */
#define THREE_D_LABS_ADAPTER    22                     /* @V3.0YEE03 */
#define NCR_ADAPTER             23                     /* @V3.0YEE03 */
#define IIT_ADAPTER             24                     /* @V3.0YEE03 */
#define APPIAN_ADAPTER          25                     /* @V3.0YEE03 */
#define SIERRA_ADAPTER          26                     /* @V3.0YEE03 */
#define CORNERSTONE_ADAPTER     27                     /* @V3.0YEE03 */
#define DIGITAL_ADAPTER         28                     /* @V3.0YEE03 */
#define COMPAQ_ADAPTER          29                     /* @V3.0YEE03 */
#define INFOTRONIC_ADAPTER      30                     /* @V3.0YEE03 */
#define OPTI_ADAPTER            31                     /* @V3.0YEE03 */
#define NULL_ADAPTER            32                     /* @V3.0JWK11 */
 /* remember to update SVGA_LASTADAPTER when adding new adapter support */

#define SVGA_FIRSTADAPTER       VIDEO7_ADAPTER         /* @V2.1YEE03 */
#define SVGA_LASTADAPTER        NULL_ADAPTER           /* @V3.0JWK11 */

/* @V2.1MNH13 start */
#ifdef FAMILY2
  #define ORCHID_MC_ADAPTER     2       /* @V2.0SEN06 */
  #define ORCHID_MC_ID          0x86    /* @V2.0SEN06 */
#endif

/*
** DAC types
*/

#define VGA_DAC           0                                     /*@V2.1MNH23*/
#define DEFAULT_DAC       VGA_DAC                               /*@V2.1MNH23*/
#define HICOLOR_DAC       1      /* Unidentified */             /*@V2.1MNH17*/
#define WINBOND_DAC       HICOLOR_DAC                           /*@V2.1MNH23*/
#define BT485_DAC         2      /* Brooktree 485 */            /*@V2.1MNH20*/
#define Weitek_BT485DAC   3      /* BT485 used by Weitek      */
#define SC15025_DAC       4      /* SC15025                   */
#define SierraDAC         SC15025_DAC /* Use more specific name! */
#define ATT490_DAC        5
#define ATTDAC            ATT490_DAC /* Use more specific name! */
#define MU9C1880_DAC      6      /* MU9C4870, MU9C1880, SS2410*/
#define MU9C4870_DAC      MU9C1880_DAC                          /*@V2.1MNH23*/

#define TRUECOLOR_DAC     7                                     /*@V2.1MNH17*/
#define IMSG173_DAC       8      /*These DACS identified, but not programmed*/
#define InmosDAC          IMSG173_DAC /* Use more specific name! */
#define CH8398_DAC        9
#define ChrontelDAC       CH8398_DAC /* Use more specific name! */
#define CIRRUS_DAC       10
#define ICS5341_DAC      11
#define SGS1702_DAC      12
#define SGSThomsonDAC16  SGS1702_DAC /* Use more specific name! */
#define SGS1703_DAC      13                                     /*@V2.1MNH23*/
#define MU9C4910_DAC     14      /* MU9C4910 (blue byte first) */
                                 /*  note that the MU9C1880 is also true color, but */
                                 /*       it does not need to be identified because */
                                 /*       behaves like the others in its group      */
#define MusicDAC16RGB    MU9C4910_DAC /* Use more specific name! */
#define ATI68830_DAC     15      /* ATI 68830 & compatible  @V2.1MNH29  */
#define S3SDAC_DAC       16      /* S3 SDAC                 @V2.1MNH29  */
#define ATT498_DAC       17      /* AT&T 498 CLOCKDAC ID=$98 no programmable clocks      @V2.1MNH29  */
#define SGS1700_DAC      18      /* SGS 1700 DAC            @V2.1MNH29  */
#define TLC34075_DAC     19      /* Texas Instruments TLC34075*/
#define ATI68875_DAC     TLC34075_DAC
#define BT476_DAC        20      /* Brooktree 476/478       @V2.1MNH29  */
#define BT478_DAC        BT476_DAC
#define BT481_DAC        21      /* Brooktree 481/482       @V2.1MNH29  */
#define BT482_DAC        BT481_DAC
#define ATI68860_DAC     22      /* ATI 68860/68880         @V2.1MNH29  */
#define MU9CBUG_DAC      23
#define CH8391_DAC       24
#define W82C490_DAC      25
#define ATT491_DAC       26
#define ATT492_DAC       27
#define ATT493_DAC       28
#define ATT497_DAC       29
#define WD90C24_DAC      30
#define MU9C9910_DAC     31      /* (blue byte first) */
#define ATT409_DAC       32          //ID=$09 16 bit pixel I/O
#define ATT499_DAC       33          //ID=$99 24 bit Pixel I/0  /*@V3.0YEE03*/
#define TI3025_DAC       34      /* TI Viewpoint 3025 CLOCKDAC*//*@V3.0YEE01*/
#define TI3020_DAC       35      /* TI Viewpoint 3020 CLOCKDAC*//*@V3.0YEE03*/
#define ICS5342_DAC      36                                     /*@V3.0YEE04*/
#define ATT408_DAC       ATT409_DAC  //ID=$09 HW pinouts diff   /*@V3.0JWK18*/
#define NULL_DAC         37                                     /*@V3.0JWK11*/
#define IMBEDDED_DAC     38      //ATI MACH64 'CT'              /*@V3.0JWK21*/
#define S3TRIO_DAC       39      /* S3 TRIO DAC               *//*@V3.0YEE05*/
#define MAX_DAC          40                                     /*@V3.0JWK21*/

#define VGADAC_NAME     "VGA_RGB"                               /*@V2.1MNH23*/
#define HICOLOR_NAME    "HICOLOR_RGB"                           /*@V2.1MNH17*/
#define BT485_NAME      "BT485_RGB"                             /*@V2.1MNH20*/
#define VIPER_NAME      "BT485_RGB"
#define SC15025_NAME    "SC15025_RGB"
#define ATT490_NAME     "ATT20C490_RGB"                         /*@V2.1MNH29*/
#define MU9C1880_NAME   "MU9C1880_BGR"
#define TRUECOLOR_NAME  "TRUECOLOR_RGB"                         /*@V2.1MNH17*/
#define IMSG173_NAME    "IMSG173_RGB"
#define CH8398_NAME     "CH8398_RGB"
#define CIRRUS_NAME     "CIRRUS_RGB"
#define ICS5341_NAME    "ICS5341_RGB"
#define SGS1702_NAME    "SGS1702_RGB"
#define SGS1703_NAME    "SGS1703_RGB"                           /*@V2.1MNH23*/
#define MU9C4910_NAME   "MU9C4910_RGB"  /* was BGR */           /*@V2.2SEN05*/
#define ATI68830_NAME   "ATI68830_RGB"                          /*@V2.1MNH20*/
#define S3SDAC_NAME     "S3SDAC_RGB"                            /*@V2.1YEE02*/
#define ATT498_NAME     "ATT20C498_RGB"                         /*@V2.1MNH29*/
#define SGS1700_NAME    "SGS1700_RGB"                           /*@V2.1YEE02*/
#define TLC34075_NAME   "TLC34075_RGB"                          /*@V2.1MNH20*/
#define ATI68875_NAME   "ATI68875_RGB"                          /*@V2.1MNH20*/
#define BT476_NAME      "BT476_RGB"                             /*@V2.1MNH20*/
#define BT481_NAME      "BT481_RGB"                             /*@V2.1MNH20*/
#define ATI68860_NAME   "ATI68860_RGB"                          /*@V2.1MNH20*/
#define MU9CBUG_NAME    "MU9C1880BUG_BGR"                       /*@V2.1MNH29*/
#define CH8391_NAME     "CH8391_RGB"                            /*@V2.1MNH29*/
#define W82C490_NAME    "W82C490_RGB"                           /*@V2.1MNH29*/
#define ATT491_NAME     "ATT20C491_RGB"                         /*@V2.1MNH29*/
#define ATT492_NAME     "ATT20C492_RGB"                         /*@V2.1MNH29*/
#define ATT493_NAME     "ATT20C493_RGB"                         /*@V2.1MNH29*/
#define ATT497_NAME     "ATT20C497_RGB"                         /*@V2.1MNH29*/
#define WD90C24_NAME    "WD90C24_RGB"                           /*@V2.1MNH35*/
#define MU9C9910_NAME   "MU9C9910_RGB"                          /*@V2.1MNH37*/
#define ATT499_NAME     "ATT20C499_RGB"                         /*@V3.0JWK18*/
#define ATT498_NAME     "ATT20C498_RGB"                         /*@V3.0JWK18*/
#define ATT409_NAME     "ATT20C409_RGB"                         /*@V2.1YEE10*/
#define TI3025_NAME     "TVP3025_RGB" /* TI Viewpoint 3025 DAC*//*@V3.0YEE01*/
#define TI3020_NAME     "TVP3020_RGB" /* TI Viewpoint 3020 DAC*//*@V3.0YEE03*/
#define ICS5342_NAME    "ICS5342_RGB"                           /*@V3.0YEE04*/
#define NULL_DAC_NAME   "NULLDAC_RGB"                           /*@V3.0JWK11*/
#define IMBEDDED_NAME   "IMBEDDED_RGB"/*ATI MACH64CT internal *//*@V3.0JWK21*/
#define S3TRIODAC_NAME  "S3_RGB"      /* S3 TRIO DAC          *//*@V3.0YEE05*/

/* NOINC */
typedef struct _DACINFO {       /* dac */
  USHORT        DACFamily;
  USHORT        DACType;
} DACINFO;

/* INC */

#define WEITEK_P9000_INCREMENT  0x0040 //Spacing between potential bases.
#define WEITEK_P9000_REGISTERS  0x0010 //Base address offset for registers.
#define WEITEK_P9000_VRAM       0x0020 //Base address offset for VRAM.
#define WEITEK_P9000_SYSCONFIG  0x0004 //Offset for config reg
#define WEITEK_P9000_INTERRUPT  0x0008 //Offset for interrupt reg
#define WEITEK_P9000_ENABLE     0x000c //Offset for interrupt enable reg
#define WEITEK_P9000_MEMCONFIG  0x0184 //Offset for memory config reg
#define WEITEK_P9000_REGLENGTH  0x2000 //Length for register addressability
/* @V2.1MNH13 end   */

#define UNKNOWN_CHIP            0

#define VIDEO7_HT205_CHIP       1
#define VIDEO7_HT208_CHIP       2
#define VIDEO7_HT209_CHIP       3
#define MAX_VIDEO7_CHIP         VIDEO7_HT209_CHIP               /*@V2.1MNH01*/

#define VIDEO7_HT205_NAME       "HT205"                         /*@V2.1MNH20*/
#define VIDEO7_HT208_NAME       "HT208"                         /*@V2.1MNH20*/
#define VIDEO7_HT209_NAME       "HT209"                         /*@V2.1MNH20*/

#define TRIDENT_8800_CHIP       1
#define TRIDENT_8900_CHIP       2
#define MAX_TRIDENT_CHIP        TRIDENT_8900_CHIP               /*@V2.1MNH01*/

#define TRIDENT_8800_NAME       "TR8800"                        /*@V2.1MNH20*/
#define TRIDENT_8900_NAME       "TR8900"                        /*@V2.1MNH20*/


#define TSENG_ET3000_CHIP       1
#define TSENG_ET4000_CHIP       2
#define TSENG_ET4000W32_CHIP    3    /* w32                   @V2.1MNH01*/
#define TSENG_ET4000W32I_CHIP   4    /* w32i (level a)        @V2.1JWK01*/
#define TSENG_ET4000W32IB_CHIP  5    /* w32i level b          @V2.1JWK01*/
#define TSENG_ET4000W32IC_CHIP  6    /* w32i level c          @V2.1JWK01*/
#define TSENG_ET4000W32PA_CHIP  7    /* w32p level a          @V2.1JWK01*/
#define TSENG_ET4000W32PB_CHIP  8    /* w32p level b          @V2.1JWK01*/
#define TSENG_ET4000W32PC_CHIP  9    /* w32p level c          @V2.1JWK01*/
#define TSENG_ET4000W32ID_CHIP 10    /* w32i level d          @V2.1JWK03*/
#define TSENG_ET4000W32PD_CHIP 11    /* w32p level d          @V2.1JWK03*/
#define TSENG_ET4000W32PX_CHIP 12    /* all others            @V2.1JWK23*/
#define MAX_TSENG_CHIP         12

#define TSENG_ET3000_NAME       "ET3000"                        /*@V2.1JWK02*/
#define TSENG_ET4000_NAME       "ET4000"                        /*@V2.1JWK02*/
#define TSENG_ET4000W32_NAME    "ET4000W32"                     /*@V2.1JWK02*/
#define TSENG_ET4000W32I_NAME   "ET4000W32IREVA"                /*@V2.1JWK02*/
#define TSENG_ET4000W32IB_NAME  "ET4000W32IREVB"                /*@V2.1JWK02*/
#define TSENG_ET4000W32IC_NAME  "ET4000W32IREVC"                /*@V2.1JWK02*/
#define TSENG_ET4000W32PA_NAME  "ET4000W32PREVA"                /*@V2.1JWK02*/
#define TSENG_ET4000W32PB_NAME  "ET4000W32PREVB"                /*@V2.1JWK02*/
#define TSENG_ET4000W32PC_NAME  "ET4000W32PREVC"                /*@V2.1JWK02*/
#define TSENG_ET4000W32ID_NAME  "ET4000W32IREVD"                /*@V2.1JWK03*/
#define TSENG_ET4000W32PD_NAME  "ET4000W32PREVD"                /*@V2.1JWK03*/
#define TSENG_ET4000W32PX_NAME  "ET4000W32PREVC_COMPATABLE"     /*@V2.1JWK23*/

#define WESTERNDIG_PVGA1A_CHIP  1
#define WESTERNDIG_WD9000_CHIP  2 /* PVGA1B */
#define WESTERNDIG_WD9011_CHIP  3 /* PVGA1C */
#define WESTERNDIG_WD9030_CHIP  4 /* PVGA1D */
#define WESTERNDIG_WD9026_CHIP  5 /* PVGA1F */                  /*@V2.1MNH01*/
#define WESTERNDIG_WD9027_CHIP  6                               /*@V2.1MNH01*/
#define WESTERNDIG_WD9031_CHIP  7 /* PVGA1DW */                 /*@V2.1MNH01*/
#define WESTERNDIG_WD9024_CHIP  8                               /*@V2.1MNH01*/
#define WESTERNDIG_WD9033_CHIP  9 /* PVGA2DW */                 /*@V2.1MNH01*/
#define MAX_WESTERNDIG_CHIP     WESTERNDIG_WD9033_CHIP          /*@V2.1MNH01*/

#define WESTERNDIG_PVGA1A_NAME  "PVGA1A"                        /*@V2.1MNH20*/
#define WESTERNDIG_WD9000_NAME  "PVGA1B"                        /*@V2.1MNH20*/
#define WESTERNDIG_WD9011_NAME  "PVGA1C"                        /*@V2.1MNH20*/
#define WESTERNDIG_WD9030_NAME  "PVGA1D"                        /*@V2.1MNH20*/
#define WESTERNDIG_WD9026_NAME  "WD90C26"                       /*@V2.1MNH20*/
#define WESTERNDIG_WD9027_NAME  "WD90C27"                       /*@V2.1MNH20*/
#define WESTERNDIG_WD9031_NAME  "WD90C31"                       /*@V2.1MNH20*/
#define WESTERNDIG_WD9024_NAME  "WD90C24"                       /*@V2.1MNH20*/
#define WESTERNDIG_WD9033_NAME  "WD90C33"                       /*@V2.1MNH20*/

#define ATI_18800_CHIP          1 /*VGAWONDER*/
#define ATI_28800_CHIP          2 /*VGAWONDER+/XL*/
#define ATI_38800_CHIP          3 /*MACH8/GraphicsUltra*/       /*@V2.1MNH01*/
#define ATI_68800_CHIP          4 /*MACH32/GraphicsUltraPro*/   /*@V2.1MNH01*/
#define ATI_88800_CHIP          5 /*MACH64*/                    /*@V2.1MNH01*/
#define ATI_88800CT_CHIP        6 /*MACH64CT*/                  /*@V3.0JWK21*/
#define MAX_ATI_CHIP            ATI_88800CT_CHIP                /*@V3.0JWK21*/

#define ATI_18800_NAME          "ATI18800"                      /*@V2.1MNH20*/
#define ATI_28800_NAME          "ATI28800"                      /*@V2.1MNH20*/
#define ATI_38800_NAME          "ATI38800MACH8"                 /*@V2.1MNH21*/
#define ATI_68800_NAME          "ATI68800MACH32"                /*@V2.1MNH21*/
#define ATI_88800_NAME          "ATI88800MACH64"                /*@V2.1MNH21*/
#define ATI_88800CT_NAME        "ATI88800MACH64CT"              /*@V3.0JWK21*/

#define IBM_SVGA_CHIP           1
#define MAX_IBM_CHIP            IBM_SVGA_CHIP                   /*@V2.1MNH01*/

#define IBM_SVGA_NAME           "IBMSVGA"                       /*@V2.1MNH20*/

#define CIRRUS_5420_CHIP        1                               /*@V2.1YEE08*/
#define CIRRUS_5422_CHIP        2
#define CIRRUS_5424_CHIP        3
#define CIRRUS_5426_CHIP        4
#define CIRRUS_5428_CHIP        5                               /*@V2.1YEE02*/
#define CIRRUS_5429_CHIP        6                               /*@V2.1YEE08*/
#define CIRRUS_543X_CHIP        7                               /*@V2.1YEE08*/
#define CIRRUS_5434_CHIP        8                               /*@V2.1YEE08*/
#define CIRRUS_6235_CHIP        9                               /*@V3.0ITO01*/
#define MAX_CIRRUS_CHIP         CIRRUS_6235_CHIP                /*@V3.0ITO01*/

#define CIRRUS_5420_NAME        "GD5420"                        /*@V2.1MNH20*/
#define CIRRUS_5422_NAME        "GD5422"                        /*@V2.1MNH20*/
#define CIRRUS_5424_NAME        "GD5424"                        /*@V2.1MNH20*/
#define CIRRUS_5426_NAME        "GD5426"                        /*@V2.1MNH20*/
#define CIRRUS_5428_NAME        "GD5428"                        /*@V2.1MNH20*/
#define CIRRUS_5429_NAME        "GD5429"                        /*@V2.1MNH20*/
#define CIRRUS_543X_NAME        "GD543X"                        /*@V2.1MNH20*/
#define CIRRUS_5434_NAME        "GD5434"                        /*@V2.1MNH20*/
#define CIRRUS_6235_NAME        "GD6235"                        /*@V3.0ITO01*/

#define S3_86C805_CHIP          1           /* 801 or 805 */
#define S3_86C928_CHIP          2
#define S3_86C911_CHIP          3           /* @V2.1YEE04 */
#define S3_86C864_CHIP          4                               /*@V2.1MNH01*/
//#define S3_DXP_CHIP              S3_86C864_CHIP               /*@IBMJ-S3GEN*/
#define S3_86C964_CHIP          5                               /*@V2.1MNH01*/
//#define S3_VXP_CHIP              S3_86C964_CHIP               /*@IBMJ-S3GEN*/

/* These chip IDs added by S3 */                                /*@V3.1MNH10*/
#define S3_86C868_CHIP          6                               /*@V3.1MNH10*/
//#define S3_V868_CHIP             S3_86C868_CHIP               /*@V3.1MNH10*/
#define S3_86C968_CHIP          7                               /*@V3.1MNH10*/
//#define S3_V968_CHIP             S3_86C968_CHIP               /*@V3.1MNH10*/
#define S3_86C732_CHIP          8                               /*@V3.1MNH10*/
//#define S3_Trio32_CHIP           S3_86C732_CHIP               /*@V3.1MNH10*/
#define S3_86C764_CHIP          9                               /*@V3.1MNH10*/
//#define S3_Trio64_CHIP           S3_86C764_CHIP               /*@V3.1MNH10*/
#define S3_86C765_CHIP         10                               /*@V3.1MNH10*/
//#define S3_V765_CHIP             S3_86C765_CHIP               /*@V3.1MNH10*/
#define S3_86CM65_CHIP         11 /* Aurora64V */               /*@V3.1MNH10*/
#define S3_86C325_CHIP         12 /* ViRGE */                   /*@V3.1MNH10*/
#define S3_86C988_CHIP         13 /* ViRGE/vX chip */           /*@V3.1MNH10*/
#define S3_86C767_CHIP         14 /* Trio64UV+ */               /*@V3.1MNH10*/
#define S3_86C765_FAMILY       15 /* Trio65V+ family compat */  /*@V3.1MNH10*/

#define S3_86C924_CHIP         16                               /*@V3.1MNH10*/
#define S3_86C866_CHIP         17                               /*@V3.1MNH10*/
//#define S3_V866_CHIP             S3_86C866_CHIP               /*@V3.1MNH10*/
#define S3_86C928PCI_CHIP      18                               /*@V3.1MNH10*/
#define MAX_S3_CHIP              S3_86C928PCI_CHIP              /*@V3.1MNH10*/

#define S3_86C805_NAME          "S386C80X"                      /*@V2.1MNH20*/
#define S3_86C928_NAME          "S386C928"                      /*@V2.1MNH20*/
#define S3_86C911_NAME          "S386C911"                      /*@V2.1MNH20*/
#define S3_86C864_NAME          "VISION864"       /* S386C864 *//*@V3.1MNH10*/
#define S3_86C964_NAME          "VISION964"       /* S386C964 *//*@V3.1MNH10*/

#define S3_86C868_NAME          "VISION868"                     /*@V3.1MNH10*/
//#define S3_V868_NAME            S3_86C868_NAME                /*@V3.1MNH10*/
#define S3_86C968_NAME          "VISION968"                     /*@V3.1MNH10*/
//#define S3_V968_NAME            S3_86C968_NAME                /*@V3.1MNH10*/
#define S3_86C732_NAME          "S3TRIO32"                      /*@V3.1MNH10*/
//#define S3_Trio32_NAME          S3_86C764_NAME                /*@V3.1MNH10*/
#define S3_86C764_NAME          "S3TRIO64"                      /*@V3.1MNH10*/
//#define S3_Trio64_NAME          S3_86C732_NAME                /*@V3.1MNH10*/
#define S3_86C765_NAME          "S3TRIO64V+"                    /*@V3.1MNH10*/
//#define S3_V765_NAME            S3_86C765_NAME                /*@V3.1MNH10*/
#define S3_86CM65_NAME          "S3AURORA64V+"                  /*@V3.1MNH10*/
#define S3_86C325_NAME          "S3VIRGE"                       /*@V3.1MNH10*/
#define S3_86C988_NAME          "S3VIRGE/VX"                    /*@V3.1MNH10*/
#define S3_86C767_NAME          "S3TRIO64UV+"                   /*@V3.1MNH10*/
#define S3_86C765_FAMILY_NAME   "S3TRIO64V+COMPATIBLE"          /*@V3.1MNH10*/

#define S3_86C924_NAME          "S386C924"                      /*@IBMJ-S3GEN*/
#define S3_86C866_NAME          "S386C866"                      /*@V3.1MNH10*/
//#define S3_V866_NAME            S3_86C866_NAME                /*@V3.1MNH10*/

#define S3_86C928PCI_NAME       "S386C928PCI"                   /*@IBMJ-S3GEN*/

#define CHIPS_FIRST_CHIP        1                /* bogus chip    @V2.1YEE09*/
#define MAX_CHIPS_CHIP          1                               /*@V2.1YEE09*/

#define WEITEK_P9000_CHIP       1                               /*@V2.1MNH01*/
#define WEITEK_W5186_CHIP       2                               /*@V2.1MNH01*/
#define WEITEK_W5286_CHIP       3                               /*@V2.1MNH01*/
#define WEITEK_P9100_CHIP       4                               /*@V2.2TSU00*/
#define MAX_WEITEK_CHIP         WEITEK_P9100_CHIP               /*@V2.2TSU00*/

#define WEITEK_P9000_NAME       "P9000"                         /*@V2.1MNH20*/
#define WEITEK_W5186_NAME       "W5186"                         /*@V2.1MNH20*/
#define WEITEK_W5286_NAME       "W5286"                         /*@V2.1MNH20*/

#define NULL_CHIP               1                               /*@V3.0JWK11*/
#define MAX_NULL_CHIP           NULL_CHIP                       /*@V3.0JWK11*/
                                                                /*@V3.0JWK11*/
#define NULL_NAME               "NULL"                          /*@V3.0JWK11*/

/*
** OEM manufacturer defines.
*/

#define UNKNOWN_MANUFACTURER    0                               /*@V2.1MNH13*/
#define DIAMOND_MANUFACTURER    1                               /*@V2.1YEE01*/
#define ORCHID_MANUFACTURER     2                               /*@V2.1MNH01*/
#define NUMBER9_MANUFACTURER    3                               /*@V2.1YEE04*/
#define ARTIST_MANUFACTURER     4                               /*@V2.1YEE07*/
#define LACUNA_MANUFACTURER     5                               /*@V2.1YEE07*/
#define STB_MANUFACTURER        6                               /*@V2.2SEN02*/
#define S3_MANUFACTURER         7                               /*@V2.1YEE10*/
#define CIRRUS_MANUFACTURER     8
#define MIROCRYSTAL_MANUFACTURER   9                            /*@V2.1YEE11*/
#define VIDEOLOGIC_MANUFACTURER    10                           /*@V2.1YEE11*/
#define THINKPAD_MANUFACTURER  11                               /*@V3.0YEE02*/
#define TOSHIBA_MANUFACTURER   12                               /*@V3.0JWK04*/
#define NEC_MANUFACTURER       13                               /*@V3.0JWK25*/
#define IBM_VISION_MANUFACTURER    14                           /*@V3.1MNH01*/
#define MANUFACTURER_MAX        IBM_VISION_MANUFACTURER         /*@V3.0MUT02*/
#define DEFAULT_MANUFACTURER    UNKNOWN_MANUFACTURER            /*@V2.2SEN02*/

/*
** OEM manufacturer MODEL defines.
*/

#define UNKNOWN_MODEL           0                               /*@V3.0JWK04*/
#define TOSHIBA_6600_MODEL      1                               /*@V3.0JWK04*/
#define TOSHIBA_2100_MODEL      2                               /*@V3.0JWK04*/
#define TOSHIBA_2100CS_MODEL    3                               /*@V3.0JWK04*/
#define TOSHIBA_2100CT_MODEL    4                               /*@V3.0JWK04*/
#define TOSHIBA_4700CS_MODEL    5                               /*@V3.0JWK04*/
#define TOSHIBA_4700CT_MODEL    6                               /*@V3.0JWK04*/
#define TOSHIBA_4800CT_MODEL    7                               /*@V3.0JWK04*/
#define TOSHIBA_4850CT_MODEL    8                               /*@V3.0JWK04*/

#define NEC_VERSA_MODEL         1                               /*@V3.0JWK25*/


/* NOINC */
typedef struct _ADAPTERS {      /* adp */
  CHAR  *Name;
  CHAR  *Manufacturer;
  INT    ID;
}ADAPTERS;

typedef CHAR *CHIPNAMES;
typedef CHIPNAMES *PCHIPNAMES;

/*
 * define CHIPS_INCL to include the following definitions.
 */
#ifdef CHIPS_INCL
/*
** This table contains adapter ID's as not to depend on the changes to
** the ID's and their order. The ChipsetName table should be kept in sync
** with the Adapters table.
*/
ADAPTERS Adapters [SVGA_LASTADAPTER + 0x01] =          /* @V2.2SENJA */
{
  {"UNKNOWN","Unknown",                         UNKNOWN_ADAPTER},
  {"VIDEO7","Headland Technology, Inc.",        VIDEO7_ADAPTER},
  {"TRIDENT","Trident Microsystems, Inc.",      TRIDENT_ADAPTER},
  {"TSENG","Tseng Labs, Inc.",                  TSENG_ADAPTER},
  {"WESTERNDIGITAL","Western Digital Corporation",WESTERNDIG_ADAPTER},
  {"ATI","ATI Technologies Inc.",               ATI_ADAPTER},
  {"IBM","IBM Corporation",                     IBM_ADAPTER},
  {"CIRRUS","Cirrus Logic, Inc.",               CIRRUS_ADAPTER},
  {"S3","S3 Incorporated",                      S3_ADAPTER},
  {"CHIPS","Chips and Techologies",             CHIPS_ADAPTER},
  {"WEITEK","WEITEK Corporation",               WEITEK_ADAPTER},
  {"NUMBER9","Number Nine Corporation",         NUMBER9_ADAPTER},
  {"GENERICPCI","Unknown",                      GENERIC_PCISVGA_ADAPTER},
  {"OAK","Oak Technology, Inc",                 OAK_ADAPTER},         /* @V3.0YEE03 */
  {"MATROX","Matrox",                           MATROX_ADAPTER},      /* @V3.0YEE03 */
  {"BROOKTREE","Brooktree Corporation",         BROOKTREE_ADAPTER},   /* @V3.0YEE03 */
  {"NVIDIA","nVIDIA",                           NVIDIA_ADAPTER},      /* @V3.0YEE03 */
  {"ALLIANCE","Alliance Semiconductor Corporation", ALLIANCE_ADAPTER},/* @V3.0YEE03 */
  {"AVANCE","Avance Logic",                     AVANCE_ADAPTER},      /* @V3.0YEE03 */
  {"MEDIAVISION","Media Vision",                MEDIAVISION_ADAPTER}, /* @V3.0YEE03 */
  {"ARKLOGIC","Ark Logic, Inc.",                ARKLOGIC_ADAPTER},    /* @V3.0YEE03 */
  {"RADIUS","Radius",                           RADIUS_ADAPTER},      /* @V3.0YEE03 */
  {"3DLABS","3D Labs Inc.",                     THREE_D_LABS_ADAPTER},/* @V3.0YEE03 */
  {"NCR","NCR Corporation",                     NCR_ADAPTER},         /* @V3.0YEE03 */
  {"IIT","IIT",                                 IIT_ADAPTER},         /* @V3.0YEE03 */
  {"APPIAN","Appian Technology",                APPIAN_ADAPTER},      /* @V3.0YEE03 */
  {"SIERRA","Sierra Semiconductor Inc.",        SIERRA_ADAPTER},      /* @V3.0YEE03 */
  {"CORNERSTONE","Cornerstone Technology",      CORNERSTONE_ADAPTER}, /* @V3.0YEE03 */
  {"DIGITAL","Digital Equipment Corporation",   DIGITAL_ADAPTER},     /* @V3.0YEE03 */
  {"COMPAQ","Compaq Computer Corporation",      COMPAQ_ADAPTER},      /* @V3.0YEE03 */
  {"INFOTRONIC","INFOTRONIC",                   INFOTRONIC_ADAPTER},  /* @V3.0YEE03 */
  {"OPTI","OPTi, Inc.",                         OPTI_ADAPTER},        /* @V3.0YEE03 */
  {"NULL","null",                               NULL_ADAPTER},        /* @V3.0JWK11 */
};
CHIPNAMES ppszUnknownChipNames [1] =
  {
    "UNKNOWN",
  };
CHIPNAMES ppszVideo7ChipNames [MAX_VIDEO7_CHIP] =               /*@V2.1MNH20*/
  { /* Video 7==Headland Technology */                          /*@V2.1MNH01*/
    VIDEO7_HT205_NAME,                                          /*@V2.1MNH20*/
    VIDEO7_HT208_NAME,                                          /*@V2.1MNH20*/
    VIDEO7_HT209_NAME,                                          /*@V2.1MNH20*/
  };
CHIPNAMES ppszTridentChipNames [MAX_TRIDENT_CHIP] =             /*@V2.1MNH20*/
  { /* Trident */                                               /*@V2.1MNH01*/
    TRIDENT_8800_NAME,                                          /*@V2.1MNH20*/
    TRIDENT_8900_NAME,                                          /*@V2.1MNH20*/
  };
CHIPNAMES ppszTsengChipNames [MAX_TSENG_CHIP] =                 /*@V2.1MNH20*/
  { /* Tseng */
    TSENG_ET3000_NAME,                                          /*@V2.1MNH01*/
    TSENG_ET4000_NAME,
    TSENG_ET4000W32_NAME,
    TSENG_ET4000W32I_NAME,
    TSENG_ET4000W32IB_NAME,
    TSENG_ET4000W32IC_NAME,                                     /*@V2.1JWK01*/
    TSENG_ET4000W32PA_NAME,                                     /*@V2.1JWK01*/
    TSENG_ET4000W32PB_NAME,                                     /*@V2.1JWK01*/
    TSENG_ET4000W32PC_NAME,                                     /*@V2.2JWK03*/
    TSENG_ET4000W32ID_NAME,                                     /*@V2.2JWK03*/
    TSENG_ET4000W32PD_NAME,                                     /*@V2.2JWK03*/
    TSENG_ET4000W32PX_NAME,                                     /*@V3.0JWK23*/
  };
CHIPNAMES ppszWDChipNames [MAX_WESTERNDIG_CHIP] =               /*@V2.1MNH20*/
  { /* Western Digital */                                       /*@V2.1MNH01*/
    WESTERNDIG_PVGA1A_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9000_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9011_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9030_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9026_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9027_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9031_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9024_NAME,                                     /*@V2.1MNH20*/
    WESTERNDIG_WD9033_NAME,                                     /*@V2.1MNH20*/
  };
CHIPNAMES ppszATIChipNames [MAX_ATI_CHIP] =                     /*@V2.1MNH20*/
  { /* ATI */                                                   /*@V2.1MNH01*/
    ATI_18800_NAME,                                             /*@V2.1MNH20*/
    ATI_28800_NAME,                                             /*@V2.1MNH20*/
    ATI_38800_NAME,                                             /*@V2.1MNH20*/
    ATI_68800_NAME,                                             /*@V2.1MNH20*/
    ATI_88800_NAME,                                             /*@V2.1MNH20*/
    ATI_88800CT_NAME,                                           /*@V3.0JWK21*/
  };
CHIPNAMES ppszIBMChipNames [MAX_IBM_CHIP] =                     /*@V2.1MNH20*/
  { /* IBM */                                                   /*@V2.1MNH01*/
    IBM_SVGA_NAME,                                              /*@V2.1MNH20*/
  };
CHIPNAMES ppszCirrusChipNames [MAX_CIRRUS_CHIP] =               /*@V2.1MNH20*/
  { /* Cirrus Logic */                                          /*@V2.1MNH01*/
    CIRRUS_5420_NAME,                                           /*@V2.1YEE31*/
    CIRRUS_5422_NAME,
    CIRRUS_5424_NAME,
    CIRRUS_5426_NAME,
    CIRRUS_5428_NAME,                                           /*@V2.1MNH01*/
    CIRRUS_5429_NAME,                                           /*@V2.1YEE31*/
    CIRRUS_543X_NAME,                                           /*@V2.1YEE31*/
    CIRRUS_5434_NAME,
    CIRRUS_6235_NAME,                                           /*@V3.0ITO01*/
  };
CHIPNAMES ppszS3ChipNames [MAX_S3_CHIP] =                       /*@V2.1MNH20*/
  { /* S3 */                                                    /*@V2.1MNH01*/
    S3_86C805_NAME,                                             /*@V2.1MNH20*/
    S3_86C928_NAME,                                             /*@V2.1MNH20*/
    S3_86C911_NAME,                                             /*@V2.1MNH20*/
    S3_86C864_NAME,                                             /*@V2.1MNH20*/
    S3_86C964_NAME,                                             /*@V2.1MNH20*/

    S3_86C868_NAME,                                             /*@V3.1MNH10*/
    S3_86C968_NAME,                                             /*@V3.1MNH10*/
    S3_86C732_NAME,                                             /*@V3.1MNH10*/
    S3_86C764_NAME,                                             /*@V3.1MNH10*/
    S3_86C765_NAME,                                             /*@V3.1MNH10*/
    S3_86CM65_NAME,                                             /*@V3.1MNH10*/
    S3_86C325_NAME,                                             /*@V3.1MNH10*/
    S3_86C988_NAME,                                             /*@V3.1MNH10*/
    S3_86C767_NAME,                                             /*@V3.1MNH10*/
    S3_86C765_FAMILY_NAME,                                      /*@V3.1MNH10*/

    S3_86C924_NAME,                                             /*@IBMJ-S3GEN*/
    S3_86C928PCI_NAME,                                          /*@IBMJ-S3GEN*/
  };
CHIPNAMES ppszWeitekChipNames [MAX_WEITEK_CHIP] =               /*@V2.1MNH20*/
  { /* Weitek */                                                /*@V2.1MNH01*/
    WEITEK_P9000_NAME,                                          /*@V2.1MNH20*/
    WEITEK_W5186_NAME,                                          /*@V2.1MNH20*/
    WEITEK_W5286_NAME,                                          /*@V2.1MNH20*/
  };
PCHIPNAMES ChipsetName [SVGA_LASTADAPTER + 0x01] =              /*@V2.1MNH20*/
{
  ppszUnknownChipNames,                                         /*@V2.1MNH20*/
  ppszVideo7ChipNames,                                          /*@V2.1MNH20*/
  ppszTridentChipNames,                                         /*@V2.1MNH20*/
  ppszTsengChipNames,                                           /*@V2.1MNH20*/
  ppszWDChipNames,                                              /*@V2.1MNH20*/
  ppszATIChipNames,                                             /*@V2.1MNH20*/
  ppszIBMChipNames,                                             /*@V2.1MNH20*/
  ppszCirrusChipNames,                                          /*@V2.1MNH20*/
  ppszS3ChipNames,                                              /*@V2.1MNH20*/
  ppszUnknownChipNames,                                         /*@V2.1MNH20*/
  ppszWeitekChipNames,                                          /*@V2.1MNH20*/
};
#endif
/*
** DIF subsystem defines
*/
typedef struct _MONMODEINFO { /* mmi */
        USHORT            usXResolution;
        USHORT            usYResolution;
        BYTE              bVertRefresh;
        BYTE              bHorizRefresh;
        BYTE              bVPolarityPos;
        BYTE              bHPolarityPos;
} MONMODEINFO;
#define MAX_PATH_NAME  256
#define MAX_MONITOR_LEN   128   /* length of name string */
#define MAX_MONITOR_MODES 10    /* up to 10 modes per monitor */
typedef struct _MonInfo { /* mi */
        CHAR             szMonitor[MAX_MONITOR_LEN];
        MONMODEINFO      ModeInfo[MAX_MONITOR_MODES];
} MONITORINFO, FAR *PMONITORINFO;

#define DIF_SERVICE_DLL_NAME   "VIDEOCFG"
#define VCFG_RESOURCE_DLL_NAME "VCFGMRI"

#define SVGA_DATAFNAME         "svgadata.pmi"
#define VIDEO_CFGFNAME         "video.cfg"
#define MONITOR_FNAME          "monitor.dif"
#define PRIVATE_MONITOR_FNAME  "private.dif"                    /*@V3.0DAI01*/

/*
**  DevEscape functions and structure for driver extra capabilities that need
**  to be configured by VIDEOCFG.DLL
*/

#define  DEVESC_QUERYDRIVERCAPS      40010L                     /*@V3.0DAI01*/
#define  DEVESC_QUERYDRIVERCAPSLIST  40011L                     /*@V3.0DAI01*/
#define  DEVESC_SETDRIVERCAPSVALUE   40012L                     /*@V3.0DAI01*/
                                                                /*@V3.0DAI01*/
#define CAPSTYPE_BOOLEAN                 1L                     /*@V3.0DAI01*/
#define CAPSTYPE_AGGREGATE_INT           2L                     /*@V3.0DAI01*/
#define CAPSTYPE_AGGREGATE_STRING        3L                     /*@V3.0DAI01*/
                                                                /*@V3.0DAI01*/
typedef struct _DRIVERCAPS                                      /*@V3.0DAI01*/
{                                                               /*@V3.0DAI01*/
   ULONG  ulCb;                                                 /*@V3.0DAI01*/
   CHAR   szCapsDesc[256];                                      /*@V3.0DAI01*/
   CHAR   szHelpFileName[256];                                  /*@V3.0DAI01*/
   ULONG  ulHelpId;                                             /*@V3.0DAI01*/
   ULONG  ulCapsType;                                           /*@V3.0DAI01*/
   ULONG  ulValueMemberSize;                                    /*@V3.0DAI01*/
   ULONG  ulNumValueMember;                                     /*@V3.0DAI01*/
   PVOID  pValueList;                                           /*@V3.0DAI01*/
   PVOID  pCurrentValue;                                        /*@V3.0DAI01*/
   PVOID  pDefaultValue;                                        /*@V3.0DAI01*/
   BOOL   bDefaultValueSupported;                               /*@V3.0DAI01*/
   BOOL   bStaticCaps;                                          /*@V3.0DAI01*/
} DRIVERCAPS, *PDRIVERCAPS;                                     /*@V3.0DAI01*/

/*
** Functions exported by VIDEOCFG.DLL.
*/

/* Begin @V2.2DAI01                                                    */
/* These functions are exported only to PM aware apps. Include PMWIN.H */
#ifdef MRESULT
BOOL EXPENTRY QueryScreenPageData(PHMODULE phmodResourceMRI,
                                  PUSHORT  pusDlgId,
                                  PSZ      pszName,
                                  PUSHORT  pusHelpId,
                                  PSZ      pszDlgProcName,
                                  BOOL     bPaletteManagedDisplay);
BOOL EXPENTRY QueryDisplayTypePageData(PHMODULE phmodResourceMRI,
                                       PUSHORT  pusDlgId,
                                       PSZ      pszName,
                                       PUSHORT  pusHelpId,
                                       PSZ      pszDlgProcName);
MRESULT EXPENTRY ScreenDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY DIF_DisplayTypeDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY DMQS_DisplayTypeDlgProc(HWND hwndDlg, USHORT msg, MPARAM mp1, MPARAM mp2);
#endif
/* End @V2.2DAI01 */

APIRET EXPENTRY QueryNumMonitors(ULONG *pulNumMonitors);
APIRET EXPENTRY GetAllMonitors(MONITORINFO *pMonitors);
APIRET EXPENTRY AddMonitorData(PSZ pszMonitorFileName, MONITORINFO *pNewMonitor);    /* @V3.0DAI01 */
APIRET EXPENTRY GetCurrentCfg(ADAPTERINFO *pAdapter,MONITORINFO *pMonitor);
APIRET EXPENTRY SetCurrentCfg(ADAPTERINFO *pAdapter,MONITORINFO *pMonitor);
BOOL   EXPENTRY GetCurrentDesktopMode(PVIDEO_ADAPTER pVideoAdapter);                 /* @V3.0DAI01 */
/*
** 16 entry point into the VIDEOCFG.DLL
*/
APIRET VIDEOAPI VCFG16Request(PVOID pIn,ULONG function,PVOID pOut1,PVOID pOut2);
typedef APIRET (VIDEOAPI  FNCFG16REQUEST)(PVOID, ULONG, PVOID, PVOID);
typedef FNCFG16REQUEST *PFNCFG16REQUEST;
/*
** functions exported to 16 bit
*/
#define VCFG_GET_CURRENT_CONFIG 0       //pIn = NULL, pOut1 = pAdapter, pOut2 = pMonitor

/* INC */


/*
** APM constants          @V3.0YEE06
*/
#define APM_RESUME              1
#define APM_SUSPEND             2
#define APM_DISABLED            4

    /* this IOCTL is used internally between videopmi and screendd @V3.0YEE06 */
#define SCREENDD_SVGA_APM               0x10    /* Wait for APM    @V3.0YEE06 */

#ifndef INCL_DOSDEVIOCTL                //@senja: bsedev.h can't be used by vvid
#ifndef BSEDEV_INCLUDED                 //don't define if bsedev already included
#define SCREENDD_SVGA_ID                0x08    /* Get SVGA info IOCTL */
#define SCREENDD_SVGA_OEM               0x09    /* Get OEM info IOCTL */
/* #define SCREENDD_SVGA_APM            0x10       Wait for APM    @V3.0YEE06 */
#define SCREENDD_CATEGORY               0x80
#define SCREENDD_NAME                   "SCREEN$"
                                           /* SCREENDD_SVGA_ID */
typedef struct _OEMSVGAINFO { /* */
        USHORT AdapterType;
        USHORT ChipType;
        ULONG  Memory;
} OEMSVGAINFO;
                                           /* SCREENDD_SVGA_OEM */
typedef struct _OEMINFO {    /* */
        ULONG  OEMLength;
        USHORT Manufacturer;
        ULONG  ManufacturerData;
        USHORT ManufacturerModel;                               /*@V3.0JWK04*/
} OEMINFO;
#endif
#endif
