; $Id: calltab.asm,v 1.2 1999-10-27 02:02:58 bird Exp $
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
    public __ldrClose@4
    public __ldrOpen@12
    public __ldrRead@24
    public __LDRQAppType@8


;
; Constants
;
MAXSIZE_PROLOG EQU 10h


CALLTAB segment
    assume cs:CALLTAB, ds:flat, ss:nothing

;
; must match with the aProcTab array in dev16\ProbKrnl.c
;
callTab:
__ldrRead@24 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrRead@24 ENDP

__ldrOpen@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrOpen@12 ENDP

__ldrClose@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrClose@4 ENDP

__LDRQAppType@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__LDRQAppType@8 ENDP

if 0
    public __LDRLoadExe
__LDRLoadExe PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__LDRLoadExe END

    public __ldrGetResource
__ldrGetResource PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrGetResource ENDP

    public __ldrOpenNewExe
__ldrOpenNewExe PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrOpenNewExe ENDP

    public __ldrCreateMte
__ldrCreateMte PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrCreateMte ENDP

    public __ldrGetMte
__ldrGetMte PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrGetMte ENDP

endif

CALLTAB ENDS

END

