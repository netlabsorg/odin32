; $Id: devSegDf.inc,v 1.1 1999-09-06 02:19:58 bird Exp $
;
; Segment definitions for win32k.sys.
;
; Copyright (c) 1998-1999 knut st. osmundsen
;


DATA16 segment word public 'FAR_DATA' use16
DATA16 ends

DATA16_BSS segment word public 'FAR_DATA' use16
DATA16_BSS ends

DATA16_CONST segment word public 'FAR_DATA' use16
DATA16_CONST ends

DATA16_GROUP    group DATA16, DATA16_CONST, DATA16_BSS

CODE16 segment word public 'CODE' use16
CODE16 ends

CODE32 segment dword public 'CODE' use32
CODE32 ends

CALLTAB segment dword public 'CODE' use32
CALLTAB ends

DGROUP group BSS32, DATA32

DATA32  segment dword public 'DATA' use32
DATA32 ends

c_common segment dword public 'DATA' use32
c_common ends

BSS32 segment dword public 'BSS' use32
BSS32 ends

CONST32_RO segment dword public 'CONST' use32
CONST32_RO ends

