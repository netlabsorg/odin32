; $Id: calltab.asm,v 1.15 2000-09-22 09:22:39 bird Exp $
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
    public _tkStartProcess
    public _f_FuStrLenZ
    public _f_FuBuff

    public _VMObjHandleInfo@12
    public _ldrASMpMTEFromHandle@4
    public __ldrOpenPath                ; exported thru assembly wrapper
    public _LDRClearSem@0
    public _ldrFindModule@16
    public _KSEMRequestMutex@8
    public _KSEMReleaseMutex@4
    public _KSEMQueryMutex@8
    public _KSEMInit@12

    public pLDRSem
    public LDRSem_offObject
    public _fpLDRSem
    public LDRSem_sel

    public pLDRLibPath
    public LDRLibPath_offObject
    public _fpLDRLibPath
    public LDRLibPath_sel

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

    public pptda_ptdasem
    public ptda_ptdasem_offObject
    public _fpptda_ptdasem
    public ptda_ptdasem_sel

    public pptda_module
    public ptda_module_offObject
    public _fpptda_module
    public ptda_module_sel

    public pptda_pBeginLIBPATH
    public ptda_pBeginLIBPATH_offObject
    public _fpptda_pBeginLIBPATH
    public pptda_pBeginLIBPATH_sel

    public pldrpFileNameBuf
    public pldrpFileNameBuf_offObject
    public _fpldrpFileNameBuf
    public pldrpFileNameBuf_sel

    public SecPathFromSFN


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
_tkStartProcess PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_tkStartProcess ENDP

; 15
_f_FuStrLenZ PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuStrLenZ ENDP

; 16
_f_FuStrLen PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuStrLen ENDP

; 17
_f_FuBuff PROC FAR
    db MAXSIZE_PROLOG dup(0cch)
_f_FuBuff ENDP

; 18
_VMObjHandleInfo@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_VMObjHandleInfo@12 ENDP

; 19
_ldrASMpMTEFromHandle@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrASMpMTEFromHandle@4 ENDP

; 20 - exported thru assembly interface due to change in build 14053.
__ldrOpenPath PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
__ldrOpenPath ENDP

; 21
_LDRClearSem@0 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_LDRClearSem@0 ENDP

; 22
_ldrFindModule@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrFindModule@16 ENDP

; 23
_KSEMRequestMutex@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_KSEMRequestMutex@8 ENDP

; 24
_KSEMReleaseMutex@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_KSEMReleaseMutex@4 ENDP

; 25
_KSEMQueryMutex@8 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_KSEMQueryMutex@8 ENDP

; 26
_KSEMInit@12 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_KSEMInit@12 ENDP

; 27
pLDRSem          dd  0
LDRSem_offObject dd  0
_fpLDRSem        dd  0
LDRSem_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)


; 28
pLDRLibPath          dd  0
LDRLibPath_offObject dd  0
_fpLDRLibPath        dd  0
LDRLibPath_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 29
_TKSuBuff@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKSuBuff@16 ENDP

; 30
_TKFuBuff@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKFuBuff@16 ENDP

; 31
_TKFuBufLen@20 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_TKFuBufLen@20 ENDP

; 32
_ldrValidateMteHandle@4 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrValidateMteHandle@4 ENDP

; 33
ppTCBCur           dd  0
pTCBCur_offObject  dd  0
_fppTCBCur         dd  0
pTCBCur_sel        dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 34
ppPTDACur          dd  0
pPTDACur_offObject dd  0
_fppPTDACur        dd  0
pPTDACur_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 35
pptda_start          dd  0
ptda_start_offObject dd  0
_fpptda_start        dd  0
ptda_start_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 36
pptda_environ          dd  0
ptda_environ_offObject dd  0
_fpptda_environ        dd  0
ptda_environ_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 37
pptda_ptdasem          dd  0
ptda_ptdasem_offObject dd  0
_fpptda_ptdasem        dd  0
ptda_ptdasem_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 38
pptda_module          dd  0
ptda_module_offObject dd  0
_fpptda_module        dd  0
ptda_module_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 39
pptda_pBeginLIBPATH           dd  0
ptda_pBeginLIBPATH_offObject  dd  0
_fpptda_pBeginLIBPATH         dd  0
pptda_pBeginLIBPATH_sel       dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 40
pldrpFileNameBuf              dd  0
pldrpFileNameBuf_offObject    dd  0
_fpldrpFileNameBuf            dd  0
pldrpFileNameBuf_sel          dw  0
    db (MAXSIZE_PROLOG - 14) dup(0cch)

; 41
SecPathFromSFN PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
SecPathFromSFN ENDP

; 42
_ldrSetVMflags@16 PROC NEAR
    db MAXSIZE_PROLOG dup(0cch)
_ldrSetVMflags@16 ENDP

CALLTAB ENDS

END

