; $Id: calltab.asm,v 1.12.4.1 2000-07-16 22:43:34 bird Exp $
;
; callTab - Call back again table - table with entry for each function or
;           variable which is overrided.
;           It holds the part of the prolog which was replaced by the jmp
;           instruction (overridden functions).
;
; Copyright (c) 1998-2000 knut st. osmundsen
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
    public _SftFileSize@8

    public _VMAllocMem@36
    public _VMGetOwner@8
    public _g_tkExecPgm
    public _f_FuStrLenZ
    public _f_FuBuff

    public _VMObjHandleInfo@12
    public _ldrOpenPath@16
    public _LDRClearSem@0
    public _KSEMRequestMutex@8

    public pLDRSem
    public LDRSem_offObject
    public _fpLDRSem
    public LDRSem_sel

    public _TKSuBuff@16
    public _TKFuBuff@16
    public _TKFuBufLen@20
    public _ldrValidateMteHandle@4

    public ppTCBCur
    public pTCBCur_offObject
    public _fppTCBCur
    public pTCBCur_sel

    public ppPTDACur
    public pPTDACur_offObject
    public _fppPTDACur
    public pPTDACur_sel

    public pptda_start
    public ptda_start_offObject
    public _fpptda_start
    public ptda_start_sel

    public pptda_environ
    public ptda_environ_offObject
    public _fpptda_environ
    public ptda_environ_sel



;
; Constants
;
MAXSIZE_PROLOG EQU 18h



CALLTAB segment
    ;assume cs:CALLTAB, ds:flat, ss:nothing
    assume ds:flat, ss:nothing
;
; callTab is an array of evt. function prologs with a jump to the real function.
; Imported and Overrided OS/2 kernel functions are called tru this table.
;
; This array of near procedures are parallel to the aImportTab array in dev16\ProbKrnl.c.
; Remember to update both!.
;
callTab:

; 0
_ldrRead@24 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrRead@24 ENDP

; 1
_ldrOpen@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrOpen@12 ENDP

; 2
_ldrClose@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrClose@4 ENDP

; 3
_LDRQAppType@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_LDRQAppType@8 ENDP

; 4
_ldrEnum32bitRelRecs@24 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrEnum32bitRelRecs@24 ENDP

; 5
_IOSftOpen@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftOpen@20 ENDP

; 6
_IOSftClose@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftClose@4 ENDP

; 7
_IOSftTransPath@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftTransPath@4 ENDP

; 8
_IOSftReadAt@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftReadAt@20 ENDP

; 9
_IOSftWriteAt@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_IOSftWriteAt@20 ENDP

; 10
_SftFileSize@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_SftFileSize@8 ENDP

; 11
_VMAllocMem@36 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMAllocMem@36 ENDP

; 12
_VMGetOwner@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMGetOwner@8 ENDP

; 13
_g_tkExecPgm PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_g_tkExecPgm ENDP

; 14
_f_FuStrLenZ PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuStrLenZ ENDP

; 15
_f_FuStrLen PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuStrLen ENDP

; 16
_f_FuBuff PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuBuff ENDP

; 17
_VMObjHandleInfo@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMObjHandleInfo@12 ENDP

; 18
_ldrOpenPath@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrOpenPath@16 ENDP

; 19
_LDRClearSem@0 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_LDRClearSem@0 ENDP

; 20
_KSEMRequestMutex@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_KSEMRequestMutex@8 ENDP

; 21
pLDRSem          dd  0
LDRSem_offObject dd  0
_fpLDRSem        dd  0
LDRSem_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 22
_TKSuBuff@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKSuBuff@16 ENDP

; 23
_TKFuBuff@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKFuBuff@16 ENDP

; 24
_TKFuBufLen@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKFuBufLen@20 ENDP

;25
_ldrValidateMteHandle@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrValidateMteHandle@4 ENDP

; 26
ppTCBCur           dd  0
pTCBCur_offObject  dd  0
_fppTCBCur         dd  0
pTCBCur_sel        dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 27
ppPTDACur          dd  0
pPTDACur_offObject dd  0
_fppPTDACur        dd  0
pPTDACur_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 28
pptda_start          dd  0
ptda_start_offObject dd  0
_fpptda_start        dd  0
ptda_start_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 29
pptda_environ          dd  0
ptda_environ_offObject dd  0
_fpptda_environ        dd  0
ptda_environ_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)





CALLTAB ENDS

END

