/*****************************************************************************
 *
 * SOURCE FILE NAME = SVGAPMI.H
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
 * DESCRIPTION  Contains VIDEOPMI specific defintions
 *
 * FUNCTIONS
 *
 * NOTES
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
 *   mm/dd/yy  @Vr.mpppxx  xxxxx  xxxxxxx
 *   02/14/96  @V3.2TSU00 148283  Merlin PBE, support linear aperture
 *   04/18/96  @V3.2TSU01 150783  Add flags for _int10handler
 *   07/15/96  @V3.2TSU02 160023  Use the old VIDEO_ADAPTER & VIDEOMDOEINFO
 *
 *****************************************************************************/


/*
 * This flag is used to set the linear aperture mode.
 * the pIn passed in PMIREQUEST_SETMODE will be the pointer to
 * (mode number | SET_LINEAR_BUFFER_MODE).
 */
#define SET_LINEAR_BUFFER_MODE  0x80000000

/*
 * definitions of bus type
 */
#define ISA_BUS    0
#define VLB_BUS    1
#define PCI_BUS    2
#define EISA_BUS   3
#define PCMCIA_BUS 4
#define MCA_BUS    5

/*
* endian type
*/
#define LITTLE_ENDIAN  0
#define BIG_ENDIAN     1


/*
** VIDEOPMI.DLL prototypes.
*/
/* NOINC */
#define HVIDEO   PVOID
typedef HVIDEO  *PHVIDEO;

#ifndef APIENTRY
#define APIENTRY /* nothing */
#endif

#ifndef APIRET
#define APIRET ULONG
#endif

typedef ULONG       MODEID;
typedef MODEID FAR *PMODEID;

/*
** PMIREQUEST_SOFTWAREINT structures @V3.2SEN01
*/
typedef struct {
  ULONG ulFlags;           //VDM initialization type
#define VDM_POSTLOAD    0x1  //adapter just loaded, used internally for initialization
#define VDM_INITIALIZE  0x2  //force initialization of a permanently open VDM, even if previously initialized
#define VDM_TERMINATE_POSTINITIALIZE   0x6  //start VDM with initialization, but close it afterwards (includes VDM_INITIALIZE)
#define VDM_QUERY_CAPABILITY     0x10  // query the current int 10 capability
#define VDM_FULL_VDM_CREATED     0x20  // a full VDM is created
#define VDM_MINI_VDM_CREATED     0x40  // a mini VDM is created
#define VDM_MINI_VDM_SUPPORTED   0x80  // mini VDM support is available
  PCHAR szName;            //VDM initialization program
  PCHAR szArgs;            //VDM initialization arguments
}INITVDM;

/*
 * Don't change these structures without
 * changing the kernel dos\dosinc\vdmint10.h !
 */
typedef struct {
  BYTE bBufferType;        // input vs output buffer
#define BUFFER_NONE     0
#define INPUT_BUFFER    1
#define OUTPUT_BUFFER   2
  BYTE bReserved;
  BYTE bSelCRF;           // CRF flag for the selector: ulong index into the CRF
  BYTE bOffCRF;           // CRF flag for the offset: ulong index into the CRF
  PVOID pAddress;         // linear address of the buffer
  ULONG ulSize;
}BUFFER, *PBUFFER;

// CRF taken from kmode.h
typedef struct vcrf_s {

        ULONG reg_eax;
        ULONG reg_ebx;
        ULONG reg_ecx;
        ULONG reg_edx;
        ULONG reg_ebp;
        ULONG reg_esi;
        ULONG reg_edi;
        ULONG reg_ds;
        ULONG reg_es;
        ULONG reg_fs;
        ULONG reg_gs;
        ULONG reg_cs;
        ULONG reg_eip;
        ULONG reg_eflag;
        ULONG reg_ss;
        ULONG reg_esp;
}VCRF;


typedef struct {
  ULONG ulBIOSIntNo;      /* 0x10 for INT10 calls */
  VCRF aCRF;
  BUFFER pB[2];
}INTCRF;
#define VPRPMI "VPRPMI$"
#define VPRSYSREQ_INT10HANDLER      1
#define VPRSYSREQ_INT10WAITFORPDB   2
#define VPRSYSREQ_MARKINT10VDM      3
#define VPRSYSREQ_MAX               VPRSYSREQ_MARKINT10VDM
/* @V3.2SEN01 end */
/*
** Structures passed as input and output parameters.
*/

typedef struct _VIDEOMODEINFO { /* vmi */
        MODEID  miModeId;               /* used to make setmode request     */

#define MODE_FLAG_NOT_MONO      0x0001  /* Mono-compatible                  */
#define MODE_FLAG_GRAPHICS      0x0002  /* Text mode, Graphics              */
#define MODE_FLAG_NO_CLR_BRST   0x0004  /* Disable Color burst              */
#define MODE_FLAG_NATIVE        0x0008  /* Native (advanced function) mode  */
#define IGNORE_CLR_BRST         0x0010  /* Disable color burst doesn't make */
                                        /* sense for this mode              */
#define NOT_PLASMA              0x0020  /* @V2.2TSU00 won't work on plasma  */
                                        /* display                          */
#define MODE_FLAG_VGA_ENTRY     0x0040  /* VGA mode, need clean up          */

#define MODE_FLAG_LINEAR_BUFFER 0x0100  /* flat frame buffer is supported   */

#define MODE_FLAG_DIRECT_RGB    0x1000  /* direct color RGB                 */
#define MODE_FLAG_DIRECT_BGR    0x2000  /* direct color BGR                 */
#define MODE_FLAG_YUV           0x4000  /* YUV color format                 */

        USHORT  usType;                 /* mode type bits                   */
        USHORT  usInt10ModeSet;         /* int 10 mode                      */
        USHORT  usXResolution;          /* horizontal pels                  */
        USHORT  usYResolution;          /* vertical scan lines              */
        ULONG   ulBufferAddress;        /* physical address of VRAM         */
        ULONG   ulApertureSize;         /* VRAM aperture                    */
        BYTE    bBitsPerPixel;          /* no of colors                     */
        BYTE    bBitPlanes;             /* number of planes                 */
        BYTE    bXCharSize;             /* font width                       */
        BYTE    bYCharSize;             /* font height                      */
        USHORT  usBytesPerScanLine;     /* logical line length in bytes     */
        USHORT  usTextRows;             /* text rows                        */
        ULONG   ulPageLength;           /* no of bytes to save a plane      */
        ULONG   ulSaveSize;             /* total bytes of VRAM to save      */
        /*
         * Monitor Info
         */
        BYTE    bVrtRefresh;
        BYTE    bHrtRefresh;
        BYTE    bVrtPolPos;
        BYTE    bHrtPolPos;
        /*
         * Direct Color Fields. The same definitions as VESA 1.2 and up.
         */
        CHAR    bRedMaskSize;               // @V3.2TSU02
        CHAR    bRedFieldPosition;          // @V3.2TSU02
        CHAR    bGreenMaskSize;             // @V3.2TSU02
        CHAR    bGreenFieldPosition;        // @V3.2TSU02
        CHAR    bBlueMaskSize;              // @V3.2TSU02
        CHAR    bBlueFieldPosition;         // @V3.2TSU02
        CHAR    bRsvdMaskSize;              // @V3.2TSU02
        CHAR    bRsvdFieldPosition;         // @V3.2TSU02

        /*
         * The number of colors can be derived from MaskSize fields.
         */
        ULONG   ulColors;             // @V3.2TSU02

        ULONG   ulReserved[3];        // @V3.2TSU02

//        USHORT  usScrnTop;           @V3.2TSU02
//        USHORT  usScrnBottom;        @V3.2TSU02
//        USHORT  usScrnLeft;          @V3.2TSU02
//        USHORT  usScrnRight;         @V3.2TSU02

//        CHAR    szColorFormat[8];    @V3.2TSU02
//        CHAR    szColorWeight[8];    @V3.2TSU02
#ifndef VIDEOPMI_30_LEVEL
        /*
        ** The assumption is that all of the base video components
        ** are built together and shipped in unison.
        ** In order to build a back-level 3.0 base video component
        ** (VIDEOPMI, BVHSVGA, VIDEOCFG, IBMGPMI), define macro
        ** VIDEOPMI_30_LEVEL.
        */
//        ULONG   ulColors;               /* @V3.2SEN01 @V3.2TSU02 */
#endif

} VIDEOMODEINFO;
typedef VIDEOMODEINFO FAR *PVIDEOMODEINFO;

typedef struct _SVGARGB { /* rgb */
        BYTE    bR;
        BYTE    bG;
        BYTE    bB;
        BYTE    bUnused;
} SVGARGB;
typedef SVGARGB FAR *PSVGARGB;

#define EIGHT_BIT_DAC   0x00010000      /* over load ulRGBCount to tell     */
                                        /* videopmi the dac values are for  */
                                        /* 8 bit DAC.                       */

typedef struct _CLUTDATA { /* clt */
        ULONG   ulRGBCount;             /* No of aRGB entries that follow   */
        ULONG   ulRGBStart;             /* Start index for RGB triplets     */
        SVGARGB aRGB[1];                /* One defined, ulRGBCount entries  */
} CLUTDATA;
typedef CLUTDATA FAR *PCLUTDATA;

typedef struct _PALETTEDATA { /* pal */
        ULONG   ulPalCount;             /* No of bPal.. entries that follow */
        ULONG   ulPalStart;             /* Start index for data             */
        BYTE    bPaletteData[1];        /* One defined, ulPalCount entries  */
} PALETTEDATA;
typedef PALETTEDATA FAR *PPALETTEDATA;

typedef struct _FONTDATA { /* font */
        ULONG   ulCharCount;            /* No of chars in font              */
        ULONG   ulFontHeight;           /* No of scanlines per character    */
        ULONG   ulFontWidth;            /* No of columns per character      */
        BYTE    bFontData[1];           /* ulCharCount*ulFontHeight entries */
} FONTDATA;
typedef FONTDATA FAR *PFONTDATA;

typedef BYTE FAR *PVRAMDATA;

typedef struct _CLEANDATA { /* clean */
    USHORT  Command;
    USHORT  IndexPort;
    USHORT  DataPort;
    USHORT  Index;
    USHORT  ANDMask;
    USHORT  ORMask;
} CLEANDATA;

typedef struct _BANKDATA { /* bank */
        MODEID  miBank;                 /*                                  */
        ULONG   ulBank;                 /*                                  */
} BANKDATA;
typedef BANKDATA FAR *PBANKDATA;

#define MAX_OEM_STRING  128
#define MAX_DAC_STRING  128
#define MAX_VERSION     128

typedef struct _ADAPTERINFO {   /* adi */
        ULONG       ulAdapterID;
        CHAR        szOEMString[MAX_OEM_STRING];    /* adapter info from Hardware section */
        CHAR        szDACString[MAX_DAC_STRING];    /* DAC info from Hardware section */
        CHAR        szRevision[MAX_VERSION];        /* whatever the revision  means   */
        ULONG       ulTotalMemory;
        ULONG       ulMMIOBaseAddress;
        ULONG       ulPIOBaseAddress;
        BYTE        bBusType;
        BYTE        bEndian;
        USHORT      usDeviceBusID;       /* has to be 2 bytes */
        USHORT      usVendorBusID;       /* has to be 2 bytes */
        USHORT      SlotID;              /* has to be 2 bytes */
} ADAPTERINFO, FAR *PADAPTERINFO;

/*
 * Adapter Instance definition. The structure will be passed in every
 * PMIREQUEST call.
 */
typedef struct _VIDEO_ADAPTER { /* va */
     HVIDEO          hvideo;

     /*
     ** The assumption is that all of the base video components
     ** are built together and shipped in unison (any driver including
     ** svgadefs.h and using the VIDEOPMI interface as well).
     **
     ** @V3.0TSU00
     ** All the callers to videopmi have to fill cbAdapterInfo and
     ** and cbVideoModeInfo with sizeof (ADAPTERINFO) and
     ** sizeof (VIDEOMODEINFO), respectively. It is for version control.
     ** All the DLLs called by videopmi have to access the ModeInfo field
     ** in VIDEO_ADAPTER by the offset of ModeInfo.
     ** For example, instead of pAdapter->ModeInfo.bBitsPerPixel,
     ** it should be
     **    ((PVIDEOMODEINFO)&((PBYTE)&(pAdapter->Adapter) + pAdapter->AdapterInfo_cb))->
     **                                              bBitsPerPixel.
     */

//
//    Goes back to the old structure of OS/2 Warp 3.0.
//
//     USHORT      AdapterInfo_cb;       @V3.2TSU02
//     USHORT      VideoModeInfo_cb;     @V3.2TSU02

     ADAPTERINFO     Adapter;
     VIDEOMODEINFO   ModeInfo;
} VIDEO_ADAPTER, FAR *PVIDEO_ADAPTER;

/*
 * prototype for import functions in PMI file
 */
typedef ULONG   *PREGS;     //pointer to registers. Passed to imported PMI functions.
typedef APIRET (EXPENTRY FNIMPORTPMI)(PVIDEO_ADAPTER, PREGS);
typedef FNIMPORTPMI *PFNIMPORTPMI;

#ifdef INCL_16
   #define VIDEOAPI FAR     //this controls the calling convention.
#else                           // 32-bit code
   #define VIDEOAPI EXPENTRY       //C calling convention
#endif

/*
** prototype for META PMI imports   @V3.2SEN01
*/
typedef APIRET (VIDEOAPI FNPMIREQ)(PVIDEO_ADAPTER, PVOID, PVOID);
typedef FNPMIREQ *PFNPMIREQ;

typedef struct _PMIREQUEST {
        PFNPMIREQ       pfnPMIRequest;
        ULONG           ulRing2Call;
} PMIREQUEST;
typedef PMIREQUEST *PPMIREQUEST;

#define METAPMI_LOADADAPTER "LoadAdapter"

typedef APIRET (EXPENTRY FNIMPORTLOADPMI)(CHAR *,PPMIREQUEST);
typedef FNIMPORTLOADPMI *PFNIMPORTLOADPMI;

/*
** prototype for VIDEOPMI request functions
*/
typedef APIRET (VIDEOAPI FNVIDEOPMIREQUEST)(PVIDEO_ADAPTER, ULONG, PVOID, PVOID);

typedef FNVIDEOPMIREQUEST     *PFNVIDEOPMIREQUEST;
                                         /* used to exchange data with vvid */
typedef struct _VDDDATA {    /* vdd */
        PFNVIDEOPMIREQUEST       npfnEntryPointAddress;
        HVIDEO                   hvideo;
} VDDDATA;
typedef VDDDATA *PVDDATA;
/* INC */
/*
** Save Restore constants
*/

#define STATEFLAG_REGISTERS     0x0001  /*                                  */
#define STATEFLAG_CLUT          0x0002  /*                                  */
#define STATEFLAG_VRAM          0x0004  /*                                  */
#define STATEFLAG_FONT          0x0008  /*                                  */

/* NOINC */
typedef struct _VIDEOSTATE { /* vst */
        ULONG     fStateFlags;          /*                                  */
        MODEID    miState;              /*                                  */
        PVOID     pModeData;            /*                                  */
        ULONG     ulVRAMSaveSize;       /* No of bytes/page to save         */
        PVRAMDATA pVRAM;                /*                                  */
        PCLUTDATA pCLUT;                /*                                  */
        PFONTDATA pFont;                /*                                  */
} VIDEOSTATE;
typedef VIDEOSTATE FAR *PVIDEOSTATE;
/* INC */

/*
** Function request parameters supported.
*/

#define PMIREQUEST_SETMODE                 0
#define PMIREQUEST_LOCKREGISTERS           1
#define PMIREQUEST_UNLOCKREGISTERS         2
#define PMIREQUEST_CLEANUP                 3
#define PMIREQUEST_SAVESTATE               4
#define PMIREQUEST_RESTORESTATE            5
#define PMIREQUEST_GETBANK                 6
#define PMIREQUEST_SETBANK                 7
#define PMIREQUEST_GETCLUT                 8
#define PMIREQUEST_SETCLUT                 9
#define PMIREQUEST_GETPALETTE             10
#define PMIREQUEST_SETPALETTE             11
#define PMIREQUEST_GETFONT                12
#define PMIREQUEST_SETFONT                13
#define PMIREQUEST_TUNEDISPLAY            14        /* @V2.2TSU00 */
#define PMIREQUEST_QUERYMAXMODEENTRIES    15
#define PMIREQUEST_QUERYMAXMODELISTSIZE   16
#define PMIREQUEST_QUERYMODEINFODATA      17
#define PMIREQUEST_QUERYMAXTRAPENTRIES    18
#define PMIREQUEST_QUERYTRAPLISTDATA      19
#define PMIREQUEST_QUERYMODEHRDWRLIST     20        /* @V2.2TSU00 */
#define PMIREQUEST_LOADPMIFILE            21
#define PMIREQUEST_IDENTIFYADAPTER        22
#define PMIREQUEST_SOFTWAREINT            23             /* @V3.2SEN01 */
#define PMIREQUEST_INT10         PMIREQUEST_SOFTWAREINT
#define PMIREQUEST_UNLOADPMIFILE          24
#define PMIREQUEST_SETMEMORYIOADDRESS     25            /* @V3.2TSU00 */

#define PMIREQUEST_MAX (PMIREQUEST_SETMEMORYIOADDRESS + 1) /* last entry + 1 */

/* NOINC */

/*
 * define PMIDEF_INCL to include the following definitions.
 */
#ifdef PMIDEF_INCL

/* @V2.2TSU00
** Possible flags in ModeAtrributes field in PMI file
** the flags will be saved in ModeInfo.ulType
*/
#define  PMI_MODE_COLOUR          0x08        /* Bit 3 - 0 = mono, 1 = colour */
#define  PMI_MODE_GRAPHICS        0x10      /* Bit 4 - 0 = text, 1 = graphics */
#define  PMI_MODE_VGA_ENTRY       0x20/* vga entry, need clean up in set mode */
#define  PMI_MODE_IGNORE_CLR_BRST 0x40 /* won't work on B/W mode              */
#define  PMI_MODE_LINEAR_APERTURE 0x80 /* linear aperture mode */

/*
 * PMI commands
 */
#define PMICMD_NONE             0xFF            //reserved
#define PMICMD_INB              0
#define PMICMD_OUTB             1
#define PMICMD_INW              2
#define PMICMD_OUTW             3
#define PMICMD_INDW             4
#define PMICMD_OUTDW            5
#define PMICMD_BINB             6
#define PMICMD_BOUTB            7
#define PMICMD_ABOUTW           8
#define PMICMD_ABOUTDW          9
#define PMICMD_RMWBN           10
#define PMICMD_RMWBI           11
#define PMICMD_RMWWN           12
#define PMICMD_REGOP           13
#define PMICMD_ASSIGN          14
/***************** for backward compatibility ***************/
#define PMICMD_WAIT            15
#define PMI_WAIT_CLEAR          0       /* Wait for CLEAR condition         */
/***************** for backward compatibility ***************/
#define PMICMD_WAITB           16
#define PMICMD_WAITW           17
#define PMICMD_WAITDW          18
#define PMICMD_READB           19               /* @V2.2TSU00          */
#define PMICMD_READW           20               /* Don't rearrange the */
#define PMICMD_READDW          21               /* order or put        */
#define PMICMD_WRITEB          22               /* anything in between */
#define PMICMD_WRITEW          23               /* for these MMIO      */
#define PMICMD_WRITEDW         24               /* commands            */
#define PMICMD_ROUTINE         25               /* @V2.2TSU01          */
#define PMICMD_LABEL           26
#define PMICMD_WHILE           27
#define PMICMD_ENDWHILE        28
#define PMICMD_IF              29
#define PMICMD_GOTO            30
#define PMICMD_STRCMP          31
#define PMICMD_MEMCMP          32
#define PMICMD_CALL            33
#define PMICMD_MAX             PMICMD_CALL  /* equals last entry   */


#define PMI_REGOP_ASSIGN          0     /* Rxxx  = <value>                  */
#define PMI_REGOP_AND             1     /* Rxxx &= <value>                  */
#define PMI_REGOP_OR              2     /* Rxxx |= <value>                  */
#define PMI_REGOP_XOR             3     /* Rxxx ^= <value>                  */
#define PMI_REGOP_PLUS            4     /* Rxxx += <value>                  */
#define PMI_REGOP_MINUS           5     /* Rxxx -= <value>                  */
#define PMI_REGOP_SHL             6     /* Rxxx <= <value>                  */
#define PMI_REGOP_SHR             7     /* Rxxx >= <value>                  */
#define PMI_REGOP_LESS            8     /* Rxx < Rxyy                       */
#define PMI_REGOP_LESS_EQUALS     9     /* Rxx <= Ryy                       */
#define PMI_REGOP_GREATER         10    /* Rxx > Rxyy                       */
#define PMI_REGOP_GREATER_EQUALS  11    /* Rxx >= Ryy                       */
#define PMI_REGOP_EQUALS          12    /* Rxx == Ryy                       */
#define PMI_REGOP_NOT_EQUALS      13    /* Rxx != Ryy                       */
#define PMI_REGOP_MAX  PMI_REGOP_NOT_EQUALS /* must equal last entry        */

/*
**      Type Definitions
**
**      Tokens are grouped, and must be kept that way.
**      There are dummy tokens, place holders, TOK_??_BEGIN and TOK_??_END
**      to make tests and expansion easier. Just be sure to keep token
**      info for a given section, within these bounds.
*/

typedef enum _PMITOKEN {        /* pt */
     TOK_SECT_BEGIN,
/***************** for backward compatibility ***************/
     TOK_ADAPTERTYPE,           /* Section Heading tokens */
     TOK_CHIPSET,
/***************** for backward compatibility ***************/
     TOK_PMIVERSION,           /* Section Heading tokens */
     TOK_COMMENT,
     TOK_MODEINFO,
     TOK_SETMODE,
     TOK_MONITORMODEINFO,
     TOK_TRAPREGS,
     TOK_UNLOCK,
     TOK_LOCK,
     TOK_CLEANUP,
     TOK_SETBANK,
     TOK_GETBANK,
     TOK_DECLARATIONS,
     TOK_COPYVRAM,
     TOK_TUNEDISPLAY,
     TOK_SETMEMORYIOADDRESS,             // @V3.2TSU00
     TOK_HARDWARE,
     TOK_IDENTIFYADAPTER,
     TOK_ISENGINEBUSY,
     TOK_ENABLECONTROLLER,
     TOK_DISABLECONTROLLER,
     TOK_SETMONITORTIMINGS,
     TOK_ROUTINE_DECLR,             /* @V2.2TSU01 */
     TOK_INCLUDE,                   /* text include */
     TOK_INCLUDECODE,               /* DLL include  */
     TOK_SECT_END,

     TOK_MI_BEGIN,
     TOK_MODEATTRIBUTES,        /* ModeInfo Section tokens */
     TOK_INT10MODESET,          /* @V2.2TSU00 int 10 mode */
     TOK_BYTESPERSCANLINE,
     TOK_TEXTROWS,              /* [ModeInfo] and [Hardware] tokens       */
     TOK_XRESOLUTION,           /* are PMI keyvariables. Do remember      */
     TOK_YRESOLUTION,           /* update the PMIKeyVarTable[] and        */
     TOK_XCHARSIZE,             /* PMI Keyvariable defines in header file */
     TOK_YCHARSIZE,
     TOK_BITSPERPIXEL,
     TOK_NUMBEROFPLANES,
     TOK_PAGELENGTH,
     TOK_SAVESIZE,
     TOK_BUFFERADDRESS,
     TOK_APERTURESIZE,
     TOK_COLORFORMAT,
     TOK_COLORWEIGHT,
     TOK_VERTICALREFRESH,
     TOK_HORIZONTALREFRESH,
     TOK_VPOLARITYPOSITIVE,
     TOK_HPOLARITYPOSITIVE,
     TOK_SCREENLEFTEDGE,
     TOK_SCREENRIGHTEDGE,
     TOK_SCREENTOPEDGE,
     TOK_SCREENBOTTOMEDGE,
     TOK_COLORS,        /* @V3.2SEN01 number of colors field added */
/***************** for backward compatibility ***************/
     TOK_INTERLACEMODE,
/***************** for backward compatibility ***************/
     TOK_MI_END,

     TOK_HARDWARE_BEGIN,      /* @V2.2TSU00 Hardware Section */
     TOK_BUSTYPE,
     TOK_OEMSTRING,
     TOK_DACSTRING,
     TOK_VERSION,
     TOK_TOTALMEMORY,
     TOK_MEMORYIOADDRESS,
     TOK_PORTIOADDRESS,
     TOK_ENDIAN,
     TOK_INT10SETMODE,                   /* @V3.1TSU00  */
     TOK_HARDWARE_END,

     TOK_TRAP_BEGIN,
     TOK_BYTE_IOPORT,             /* @V2.2TSU00, trap registers type */
     TOK_WORD_IOPORT,
     TOK_DWORD_IOPORT,
     TOK_BYTE_MMIOPORT,
     TOK_WORD_MMIOPORT,
     TOK_DWORD_MMIOPORT,
     TOK_RESET,
     TOK_INDEX,
     TOK_INDEXMASK,
     TOK_DATAMASK,
     TOK_ACCEL,
     TOK_RO,
     TOK_WO,
     TOK_RW,
     TOK_TRAP_END,

     TOK_SM_BEGIN,
     TOK_INB,                /* Command Section tokens */
     TOK_OUTB,
     TOK_INW,
     TOK_INDW,
     TOK_OUTW,
     TOK_OUTDW,
     TOK_BINB,
     TOK_BOUTB,
     TOK_RMWBI,
     TOK_RMWBN,
     TOK_RMWWN,
     TOK_ABOUTW,
     TOK_ABOUTDW,
     TOK_IF,
     TOK_GOTO,
     TOK_VARIABLE,
     TOK_WHILE,
     TOK_ENDWHILE,
     TOK_MEMCMP,
     TOK_STRCMP,
     TOK_READB,         /* @V2.2TSU00          */
     TOK_READW,         /* Don't rearrange the */
     TOK_READDW,        /* order or put        */
     TOK_WRITEB,        /* anything in between */
     TOK_WRITEW,        /* for these MMIO      */
     TOK_WRITEDW,       /* commands            */
     TOK_CALL,
     TOK_ROUTINE,
/***************** for backward compatibility ***************/
     TOK_WAIT,
/***************** for backward compatibility ***************/
     TOK_WAITB,
     TOK_WAITW,
     TOK_WAITDW,
     TOK_VALUE,
     TOK_PMIREG,
     TOK_LPAREN,
     TOK_RPAREN,
     TOK_LBRACE,
     TOK_RBRACE,
     TOK_EQUALS,
     TOK_NOTEQUALS,
     TOK_SEMICOLON,
     TOK_COMMA,
     TOK_COLON,
     TOK_MINUS,
     TOK_HYPHEN = TOK_MINUS,
     TOK_NEGATE,         /* @V2.2TSU01 */
     TOK_DOUBLEQUOTE,
     TOK_PLUS,
     TOK_REGOP_AND,
     TOK_REGOP_OR,
     TOK_REGOP_XOR,
     TOK_REGOP_SHL,
     TOK_LESS = TOK_REGOP_SHL,  /* @V2.2TSU01 */
     TOK_REGOP_SHR,
     TOK_SM_END,

     TOK_EOF,
     TOK_IOPORT,
     TOK_MMIO,
     TOK_PIO,
     TOK_ISA,
     TOK_VLB,
     TOK_PCI,
     TOK_EISA,
     TOK_PCMCIA,
     TOK_MCA,
     TOK_BIG_ENDIAN,
     TOK_LITTLE_ENDIAN,
     TOK_QUOTEDSTRING,
     TOK_MMIO_REG_NAME,
     TOK_IOPORT_NAME,
     TOK_PMI_KEYVAR,
     TOK_SHADOWVRAM,
     TOK_MINIVDM,                /* @V3.1TSU00  */
     TOK_FULLVDM,                /* @V3.1TSU00  */
     TOK_ERROR,                /* error */
     TOK_PASS
} PMITOKEN;

typedef struct _TOK { /* tok */
  CHAR *tok_txt;
  PMITOKEN tok_val;
} TOK;

/*
 * definitions of bus types.
 */
#define ISA_BUS    0           /* @V2.2SENJA */
#define VLB_BUS    1
#define PCI_BUS    2
#define EISA_BUS   3
#define PCMCIA_BUS 4
#define MCA_BUS    5
#define BUS_MAX MCA_BUS

#endif /* PMIDEF_INCL */

/*
 * PMI token declarations
 * PMI_TOKEN_ARRAY should be defined to include the following array.
 * In this way, this array will not be allocated for every C file.
 */
#ifdef PMI_TOKEN_ARRAY

TOK Tokens[] =
{

/***************** for backward compatibility ***************/
  "[ADAPTERTYPE]",      TOK_ADAPTERTYPE,
  "[CHIPSET]",          TOK_CHIPSET,
/***************** for backward compatibility ***************/
  "[PMIVERSION]",       TOK_PMIVERSION,
  "[COMMENT]",          TOK_COMMENT,
  "[MODEINFO]",         TOK_MODEINFO,
  "[SETMODE]",          TOK_SETMODE,
  "[MONITORMODEINFO]",  TOK_MONITORMODEINFO,
  "[TRAPREGS]",         TOK_TRAPREGS,
  "[UNLOCK]",           TOK_UNLOCK,
  "[LOCK]",             TOK_LOCK,
  "[CLEANUP]",          TOK_CLEANUP,
  "[SETBANK]",          TOK_SETBANK,
  "[GETBANK]",          TOK_GETBANK,
  "[DECLARATIONS]",     TOK_DECLARATIONS,
  "[COPYVRAM]",         TOK_COPYVRAM,
  "[TUNEDISPLAY]",      TOK_TUNEDISPLAY,
  "[SETMEMORYIOADDRESS]",TOK_SETMEMORYIOADDRESS,  // @V3.2TSU00
  "[HARDWARE]",         TOK_HARDWARE,
  "[IDENTIFYADAPTER]",  TOK_IDENTIFYADAPTER,
  "[ISENGINEBUSY]",     TOK_ISENGINEBUSY,
  "[ENABLECONTROLLER]", TOK_ENABLECONTROLLER,
  "[DISABLECONTROLLER]",TOK_DISABLECONTROLLER,
  "[SETMONITORTIMINGS]",TOK_SETMONITORTIMINGS,
  "INCLUDE",            TOK_INCLUDE,                   /* text include */
  "INCLUDECODE",        TOK_INCLUDECODE,               /* DLL include  */
  /*
   * Hardware Section
   */
  "BUSTYPE",            TOK_BUSTYPE,
  "OEMSTRING",          TOK_OEMSTRING,
  "DACSTRING",          TOK_DACSTRING,
  "VERSION",            TOK_VERSION,
  "TOTALMEMORY",        TOK_TOTALMEMORY,
  "MEMORYIOADDRESS",    TOK_MEMORYIOADDRESS,
  "PORTIOADDRESS",      TOK_PORTIOADDRESS,
  "ENDIAN",             TOK_ENDIAN,
  "INT10SETMODE",       TOK_INT10SETMODE,    /* @V3.1TSU00  */
  /*
   * TrapRegs Section
   */
  "BYTE_IOPORT",        TOK_BYTE_IOPORT,
  "WORD_IOPORT",        TOK_WORD_IOPORT,
  "DWORD_IOPORT",       TOK_DWORD_IOPORT,
  "BYTE_MMIOPORT",      TOK_BYTE_MMIOPORT,
  "WORD_MMIOPORT",      TOK_WORD_MMIOPORT,
  "DWORD_MMIOPORT",     TOK_DWORD_MMIOPORT,
  "RESET",              TOK_RESET,
  "INDEX",              TOK_INDEX,
  "INDEXMASK",          TOK_INDEXMASK,
  "DATAMASK",           TOK_DATAMASK,
  "ACCEL",              TOK_ACCEL,
  "RO",                 TOK_RO,
  "WO",                 TOK_WO,
  "RW",                 TOK_RW,
  /*
   * ModeInfo Section
   */
  "INT10MODESET",       TOK_INT10MODESET,
  "MODEATTRIBUTES",     TOK_MODEATTRIBUTES,
  "BYTESPERSCANLINE",   TOK_BYTESPERSCANLINE,
  "TEXTROWS",           TOK_TEXTROWS,
  "XRESOLUTION",        TOK_XRESOLUTION,
  "YRESOLUTION",        TOK_YRESOLUTION,
  "XCHARSIZE",          TOK_XCHARSIZE,
  "YCHARSIZE",          TOK_YCHARSIZE,
  "BITSPERPIXEL",       TOK_BITSPERPIXEL,
  "NUMBEROFPLANES",     TOK_NUMBEROFPLANES,
  "PAGELENGTH",         TOK_PAGELENGTH,
  "SAVESIZE",           TOK_SAVESIZE,
  "BUFFERADDRESS",      TOK_BUFFERADDRESS,
  "APERTURESIZE",       TOK_APERTURESIZE,
  "COLORFORMAT",        TOK_COLORFORMAT,
  "COLORWEIGHT",        TOK_COLORWEIGHT,
/***************** for backward compatibility ***************/
  "INTERLACEMODE",      TOK_INTERLACEMODE,
/***************** for backward compatibility ***************/
  "VERTICALREFRESH",    TOK_VERTICALREFRESH,
  "HORIZONTALREFRESH",  TOK_HORIZONTALREFRESH,
  "VPOLARITYPOSITIVE",  TOK_VPOLARITYPOSITIVE,
  "HPOLARITYPOSITIVE",  TOK_HPOLARITYPOSITIVE,
  "SCREENLEFTEDGE",     TOK_SCREENLEFTEDGE,
  "SCREENRIGHTEDGE",    TOK_SCREENRIGHTEDGE,
  "SCREENTOPEDGE",      TOK_SCREENTOPEDGE,
  "SCREENBOTTOMEDGE",   TOK_SCREENBOTTOMEDGE,
  "COLORS",             TOK_COLORS,    /* @V3.2SEN01 */
  /*
   * PMI Commands
   */
  "INB",                TOK_INB,
  "OUTB",               TOK_OUTB,
  "INW",                TOK_INW,
  "OUTW",               TOK_OUTW,
  "INDW",               TOK_INDW,
  "OUTDW",              TOK_OUTDW,
  "BINB",               TOK_BINB,
  "BOUTB",              TOK_BOUTB,
  "RMWBI",              TOK_RMWBI,
  "RMWBN",              TOK_RMWBN,
/***************** for backward compatibility ***************/
  "RMWB",               TOK_RMWBI,
  "RMWW",               TOK_RMWBN,
/***************** for backward compatibility ***************/
  "RMWWN",               TOK_RMWWN,
  "ABOUTW",             TOK_ABOUTW,
  "ABOUTDW",            TOK_ABOUTDW,
  "IF",                 TOK_IF,
  "GOTO",               TOK_GOTO,
  "WHILE",              TOK_WHILE,
  "ENDWHILE",           TOK_ENDWHILE,
  "MEMCMP",             TOK_MEMCMP,
  "STRCMP",             TOK_STRCMP,
  "READB",              TOK_READB,      /* @V2.2TSU00          */
  "READW",              TOK_READW,      /* Don't rearrange the */
  "READDW",             TOK_READDW,     /* order or put        */
  "WRITEB",             TOK_WRITEB,     /* anything in between */
  "WRITEW",             TOK_WRITEW,     /* for these MMIO      */
  "WRITEDW",            TOK_WRITEDW,    /* commands            */
  "CALL",               TOK_CALL,
/***************** for backward compatibility ***************/
  "WAIT",               TOK_WAIT,
/***************** for backward compatibility ***************/
  "WAITB",              TOK_WAITB,
  "WAITW",              TOK_WAITW,
  "WAITDW",             TOK_WAITDW,
  /*
   * others
   */
  "MMIO",               TOK_MMIO,    /* used in [Declarations]  */
/***************** for backward compatibility ***************/
  "P",                  TOK_MMIO,    /* used in [Declarations]  */
/***************** for backward compatibility ***************/
  "PIO",                TOK_PIO,     /*                         */
  "ISA",                TOK_ISA,
  "VLB",                TOK_VLB,
  "PCI",                TOK_PCI,
  "EISA",               TOK_EISA,
  "PCMCIA",             TOK_PCMCIA,
  "MCA",                TOK_MCA,
  "SHADOWVRAM",         TOK_SHADOWVRAM,
  "BIG",                TOK_BIG_ENDIAN,
  "LITTLE",             TOK_LITTLE_ENDIAN,
  "MINIVDM",            TOK_MINIVDM,         /* @V3.1TSU00  */
  "FULLVDM",            TOK_FULLVDM          /* @V3.1TSU00  */

} ;

extern TOK Tokens[];

#endif  /* PMI_TOKEN_ARRAY */

/*
** PMI subsystem errors
*/
#define ERROR_ADAPTER_NOT_SUPPORTED          ERROR_USER_DEFINED_BASE+0x10
#define ERROR_REFRESH_NOT_SUPPORTED          ERROR_USER_DEFINED_BASE+0x11
#define ERROR_MODE_NOT_SUPPORTED             ERROR_USER_DEFINED_BASE+0x12
#define ERROR_REQ_SECTION_UNDEFINED          ERROR_USER_DEFINED_BASE+0x13
#define ERROR_PMI_FILE_SYNTAX                ERROR_USER_DEFINED_BASE+0x14
#define ERROR_NO_MONITOR_SUPPORT             ERROR_USER_DEFINED_BASE+0x15
#define ERROR_INVALID_CONFIGURATION          ERROR_USER_DEFINED_BASE+0x16
/* @V3.2SEN01 start */
#define ERROR_INADEQUATE_VDM_SUPPORT       ERROR_USER_DEFINED_BASE+0x17
#define ERROR_VDM_CREATION_NOT_SHELLPROCESS    ERROR_USER_DEFINED_BASE+0x18
#define ERROR_MINIVDM_PROCESSSUPPORTONLY   ERROR_USER_DEFINED_BASE+0x19
#define ERROR_INVALID_ADAPTER_EXTENSION      ERROR_USER_DEFINED_BASE+0x20
#define ERROR_ADAPTER_STILL_IN_USE           ERROR_USER_DEFINED_BASE+0x21
#define ERROR_UPLEVEL_VIDEOPMI               ERROR_USER_DEFINED_BASE+0x22  //structure change for cb and ulcolors
/* @V3.2SEN01 end */

/* INC */

