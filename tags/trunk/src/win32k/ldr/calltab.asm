; $Id: calltab.asm,v 1.5 2000-02-18 19:27:30 bird Exp $
;
; callTab - Call back again table - table with entry for each function which is overrided.
;           It holds the part of the prolog which was replaced by a jmp instruction.
;
; Copyright (c) 1998-1999 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc

;
; Exported symbols
;
    public callTab
    public _ldrClose@4
    public _ldrOpen@12
    public _ldrRead@24
    public _LDRQAppType@8
    public _ldrEnum32bitRelRecs@24

    public _IOSftOpen@20
    public _IOSftClose@4
    public _IOSftTransPath@4
    public _IOSftReadAt@20
    public _IOSftWriteAt@20

    public _VMAllocMem@36
    public _VMGetOwner@8
    public g_tkExecPgm


;
; Constants
;
MAXSIZE_PROLOG EQU 10h



CALLTAB segment
    assume cs:CALLTAB, ds:flat, ss:nothing
;
; callTab is an array of evt. function prologs with a jump to the real function.
; Imported and Overrided OS/2 kernel functions are called tru this table.
;
; This array of near procedures are parallel to the aProcTab array in dev16\ProbKrnl.c.
; Remember to update both!.
;
callTab:
_ldrRead@24 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrRead@24 ENDP

_ldrOpen@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrOpen@12 ENDP

_ldrClose@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrClose@4 ENDP

_LDRQAppType@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_LDRQAppType@8 ENDP

_ldrEnum32bitRelRecs@24 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrEnum32bitRelRecs@24 ENDP


_IOSftOpen@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftOpen@20 ENDP

_IOSftClose@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftClose@4 ENDP

_IOSftTransPath@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftTransPath@4 ENDP

_IOSftReadAt@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftReadAt@20 ENDP

_IOSftWriteAt@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftWriteAt@20 ENDP


_VMAllocMem@36 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMAllocMem@36 ENDP

_VMGetOwner@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMGetOwner@8 ENDP

g_tkExecPgm PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
g_tkExecPgm ENDP

CALLTAB ENDS

END

