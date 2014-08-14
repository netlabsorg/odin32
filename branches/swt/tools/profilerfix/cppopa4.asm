; $Id: cppopa4.asm,v 1.4 2002-05-02 23:15:38 bird Exp $
;

;
; Copyright 1990-1998 IBM Corp.
;

;
; File Name         : G:\fun\cpppan40.obj\cpppan40.obj
; Format            : Object Module Format (OMF/IBM)
; Module name       : cppopa4.asm
; Translator        : ALP: Assembly Language Processor  Ver 4.00.002
; Debug info type   : Unknown



        .586
        .MODEL FLAT

;
;   Defined Constants And Macros
;

;
; FS selector fixes.
;
;FS_FIXES_1      EQU     1               ; Level 1 - Load correct fs when ever it's accessed
                                        ;           And save it before calling Dos* and initAddr.
;FS_FIXES_2      EQU     1               ; Level 2 - Save and load correct fs before every
                                        ;           indirect call.
FS_FIXES_3      EQU     1               ; Level 3 - Save and load correct fs upon entry.
                                        ;
ifdef FS_FIXES_3
FS_FIXES_STACK  EQU     4
else
FS_FIXES_STACK  EQU     0
endif


large           EQU                     ; workaround for IDA code.



;
;   Segment definitionas.
;
DATA32          segment dword public 'DATA' use32
DATA32          ends
CONST32         segment dword public 'DATA' use32
CONST32         ends
BSS32           segment dword public 'DATA' use32
BSS32           ends

DGROUP          group DATA32, CONST32, BSS32

CODE32          segment dword public 'CODE' use32
CODE32          ends


;
;   Externals
;
                extrn DosExit:near
                extrn DosSleep:far
                extrn DosLoadModule:near
                extrn DosQueryProcAddr:near
                extrn DosUnwindException:near
ifdef FS_FIXES_1
                extrn Dos32TIB:abs
elseifdef FS_FIXES_2
                extrn Dos32TIB:abs
elseifdef FS_FIXES_3
                extrn Dos32TIB:abs
endif


;
;   Global Variables
;
DATA32          segment
;               assume cs:DATA32
                public $$PROSEGPTR32
$$PROSEGPTR32   dd 0                    ; DATA XREF: $$PROFCALL32+5r
                                        ; $$PROFCALL32+2Fr ...
initAdr         dd 0                    ; DATA XREF: DoInit+27o
                                        ; DoInit+63r
alreadyTriedInit dd 0                   ; DATA XREF: $$PROFCALL32+13r
                                        ; $$PROFCALL32+25w ...
DATA32          ends


;
;   Structures and Typedefs
;
PROSEG          struc
    proseg_flags            dw ?
    proseg_unused           dw ?
    proseg_rc               dd ?
    proseg_rcSys            dd ?
    proseg_clkIntCtr        dd ?
    proseg_initTics_low     dd ?
    proseg_initTics_high    dd ?
    proseg_exitTics_low     dd ?
    proseg_exitTics_high    dd ?
    proseg_preTics_low      dd ?
    proseg_preTics_high     dd ?
    proseg_postTics_low     dd ?
    proseg_postTics_high    dd ?
    proseg_clockFrequency_low dd ?
    proseg_clockFrequency_high dd ?
    proseg_logBuf1st        dd ?
    proseg_logBufMax        dd ?
    proseg_logBufNext       dd ?
    proseg_logBufWrap       dd ?
if 0
    proseg_procAddrs        dd 7 dup(?)
else
    proseg_pfnINIT_PROSEG   dd ?        ; 00h + proseg_procAddrs
    proseg_pfnINIT_IDT      dd ?        ; 04h + proseg_procAddrs
    proseg_pfnINIT_ID4T     dd ?        ; 08h + proseg_procAddrs
    proseg_pfnINIT_ID44     dd ?        ; 0ch + proseg_procAddrs
    proseg_pfnINIT_ID44T    dd ?        ; 10h + proseg_procAddrs
    proseg_pfnUnknown1      dd ?        ; 14h + proseg_procAddrs
    proseg_pfnUnknown2      dd ?        ; 18h + proseg_procAddrs
endif
    proseg_maxTid           dd ?
    proseg_ptrCurTid        dd ?
    proseg_maxSuffixBytes   dd ?
    proseg_usedSuffixBytes  dd ?
    proseg_ptrSuffixBytes   dd ?
    proseg_ticsEntry        dd ?
    proseg_ticsExit         dd ?
    proseg_ticsUser         dd ?
    proseg_curTidNum        dd ?
    proseg_extraDDH         dd ?
    proseg_appDDH           dd ?
    proseg_logDev           db 16 dup(?)
    proseg_logDLL           db 16 dup(?)
    proseg_procNames        db 112 dup(?)
    _counterArray           dd 9 dup(?)
    _command                dd ?
    _parm                   dd 3 dup(?)
    proseg_retStacks        dd ?
    proseg_tidDecNest       dd ?
    proseg_tidMaxNest       dd ?
    proseg_tidTrigger       dd ?
    conditionalExit         dd ?
    _cpuType                dd ?
    _lockFlag               dd ?
    proseg_reserved         dd 2 dup(?)
PROSEG          ends

TVersionId      struc
    _versionNumber      dd ?
    _releaseNumber      dd ?
    _modificationLevel  dd ?
    _prosegSize         dd ?
TVersionId      ends

TStackHeader    struc
    _offsetOfTop        dd ?
TStackHeader    ends

TStackEntry     struc
    _returnAddr         dd ?
    _frameAddr          dd ?
TStackEntry     ends

tib2_s          struc
    tib2_ultid          dd ?
    tib2_ulpri          dd ?
    tib2_version        dd ?
    tib2_usMCCount      dw ?
    tib2_fMCForceFlag   dw ?
tib2_s          ends

tib_s           struc
    tib_pexchain        dd ?
    tib_pstack          dd ?
    tib_pstacklimit     dd ?
    tib_ptib2           dd ?
    tib_version         dd ?
    tib_arbpointer      dd ?
tib_s           ends

pib_s           struc
    pib_ulpid          dd ?
    pib_ulppid         dd ?
    pib_hmte           dd ?
    pib_pchcmd         dd ?
    pib_pchenv         dd ?
    pib_flstatus       dd ?
    pib_ultype         dd ?
pib_s           ends


;
;   Code!
;
CODE32          segment
;                assume cs:CODE32
;                assume cs:FLAT
                assume es:nothing, ss:nothing, ds:FLAT, fs:nothing, gs:nothing

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл
                public _ProfileHook32
_ProfileHook32  proc near               ; CODE XREF: $$PROFSETJMPp longjmpp
                                        ; DATA XREF: ...
                retn
_ProfileHook32  endp


;
; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл
;
                public $$PROFCALL32
$$PROFCALL32    proc near               ; CODE XREF: eCal+23j uCal+5p

var_14          = dword ptr -14h
arg_0           = dword ptr  4

                pushfd
                push    ebx
                push    esi
                push    edx
                push    eax
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                mov     esi, $$PROSEGPTR32
                ASSUME  esi:ptr PROSEG
                or      esi, esi
                jnz     short loc_6_4E
                nop
                nop
                nop
                nop
                mov     eax, alreadyTriedInit
                or      eax, eax
                jnz     loc_6_FD
                call    DoInit
                mov     alreadyTriedInit, 1
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      loc_6_FD

loc_6_4E:                               ; CODE XREF: $$PROFCALL32+Dj
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                test    dword ptr [esi.proseg_flags], 10h
                jnz     loc_6_FD
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi.proseg_maxTid]
                jnb     loc_6_FD
                mov     eax, [esi.conditionalExit]
                mov     dword ptr [eax+ebx*4], 0FFFFFFFFh
                mov     eax, [esi.proseg_tidTrigger]
                cmp     dword ptr [eax+ebx*4], 0
                jz      loc_6_FD
                nop
                nop
                nop
                nop
                mov     eax, [esi.proseg_tidDecNest]
                dec     dword ptr [eax+ebx*4]
                js      loc_6_104
                nop
                nop
                nop
                nop
                push    edi
                lea     eax, [esp+18h + FS_FIXES_STACK]
                call    verifyStackFrame
                or      eax, eax
                jnz     short loc_6_103
                mov     eax, offset $$PROFRET32
                xchg    eax, [esp+18h+arg_0 + FS_FIXES_STACK]
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi.proseg_retStacks]
                push    eax
                lea     eax, [esp+1Ch + FS_FIXES_STACK]
                call    checkForUnwindAtCall
                pop     eax
                add     dword ptr [edi], 8
                add     edi, [edi]
                mov     [edi], eax
                lea     eax, [esp+18h + FS_FIXES_STACK]
                mov     [edi+4], eax
                mov     eax, [eax]
                test    dword ptr [esi.proseg_flags], 20h
                jz      short loc_6_F4
                nop
                nop
                nop
                nop
                shl     ebx, 10h
                mov     bl, 1
                mov     edx, [esp+18h+var_14 + FS_FIXES_STACK]
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif
                jmp     short loc_6_FC
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
;                align 4
                nop
                nop
                nop

loc_6_F4:                               ; CODE XREF: $$PROFCALL32+CCj
                shl     ebx, 10h
                mov     bl, 40h ; '@'
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif

loc_6_FC:                               ; CODE XREF: $$PROFCALL32+DEj
                pop     edi

loc_6_FD:                               ; CODE XREF: $$PROFCALL32+1Aj
                                        ; $$PROFCALL32+37j ...
ifdef FS_FIXES_3
                pop     fs
endif
                pop     eax
                pop     edx
                pop     esi
                pop     ebx
                popfd
                retn
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_103:                              ; CODE XREF: $$PROFCALL32+95j
                pop     edi

loc_6_104:                              ; CODE XREF: $$PROFCALL32+83j
                mov     eax, [esi.proseg_tidDecNest]
                inc     dword ptr [eax+ebx*4]
                jmp     short loc_6_FD
$$PROFCALL32    endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public $$PROFRET32
$$PROFRET32     proc near
;                assume esi:nothing

;var_8          = byte ptr -8
var_8           = dword ptr -8
var_4           = dword ptr -4

                lea     esp, [esp-4]
                pushfd
                push    eax
                push    ebx
                push    esi
                push    edi
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     esi, $$PROSEGPTR32
                movzx   ebx, word ptr [ebx]
                dec     ebx
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks
                push    eax
                lea     eax, [esp+18h+var_4 + FS_FIXES_STACK]
                call    checkForUnwindAtReturn
                pop     eax
                sub     dword ptr [edi], 8
                add     edi, [edi]
                mov     eax, [edi+8]
                mov     [esp+14h + FS_FIXES_STACK], eax
                shl     ebx, 10h
                mov     bl, 0
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_IDT]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_IDT]
endif
ifdef FS_FIXES_3
                pop     fs
endif
                pop     edi
                pop     esi
                pop     ebx
                pop     eax
                popfd
                retn
$$PROFRET32     endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public $$PROFTRIG32
$$PROFTRIG32    proc near

var_14          = dword ptr -14h
arg_0           = dword ptr  4

                pushfd
                push    ebx
                push    esi
                push    edx
                push    eax
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jnz     short loc_6_19F
                nop
                nop
                nop
                nop
                mov     eax, alreadyTriedInit
                or      eax, eax
                jnz     loc_6_24A
                call    DoInit
                mov     alreadyTriedInit, 1
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      loc_6_24A

loc_6_19F:                              ; CODE XREF: $$PROFTRIG32+Dj
                test    dword ptr [esi].proseg_flags, 10h
                jnz     loc_6_24A
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi].proseg_maxTid
                jnb     loc_6_24A
                mov     eax, [esi].conditionalExit
                mov     dword ptr [eax+ebx*4], 0FFFFFFFFh
                mov     eax, [esi].proseg_tidDecNest
                dec     dword ptr [eax+ebx*4]
                js      loc_6_251
                push    edi
                lea     eax, [esp+18h + FS_FIXES_STACK]
                call    verifyStackFrame
                or      eax, eax
                jnz     loc_6_250
                mov     eax, offset $$PROFTRIGRET32
                xchg    eax, [esp+18h+arg_0 + FS_FIXES_STACK]
                cmp     eax, offset $$PROFTRIGRET32
                jz      short loc_6_250
                mov     edi, [esi].proseg_tidTrigger
                inc     dword ptr [edi+ebx*4]
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks
                push    eax
                lea     eax, [esp+1Ch + FS_FIXES_STACK]
                call    checkForUnwindAtCall
                pop     eax
                add     dword ptr [edi], 8
                add     edi, [edi]
                mov     [edi], eax
                lea     eax, [esp+18h + FS_FIXES_STACK]
                mov     [edi+4], eax
                mov     eax, [eax]
                test    dword ptr [esi].proseg_flags, 20h
                jz      short loc_6_241
                nop
                nop
                nop
                nop
                shl     ebx, 10h
                mov     bl, 1
                mov     edx, [esp+18h+var_14 + FS_FIXES_STACK]
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif
                jmp     short loc_6_249
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
                ; align 4 ??
                nop
                nop
                nop

; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_241:                              ; CODE XREF: $$PROFTRIG32+C8j
                shl     ebx, 10h
                mov     bl, 40h ; '@'
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif

loc_6_249:                              ; CODE XREF: $$PROFTRIG32+DAj
                pop     edi

loc_6_24A:                              ; CODE XREF: $$PROFTRIG32+1Aj
                                        ; $$PROFTRIG32+37j ...
ifdef FS_FIXES_3
                pop     fs
endif
                pop     eax
                pop     edx
                pop     esi
                pop     ebx
                popfd
                retn
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_250:                              ; CODE XREF: $$PROFTRIG32+81j
                                        ; $$PROFTRIG32+91j
                pop     edi

loc_6_251:                              ; CODE XREF: $$PROFTRIG32+73j
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                jmp     short loc_6_24A
$$PROFTRIG32    endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public $$PROFTRIGRET32
$$PROFTRIGRET32 proc near

;var_8          = byte ptr -8
var_8           = dword ptr -8
var_4           = dword ptr -4

                lea     esp, [esp-4]
                pushfd
                push    eax
                push    ebx
                push    esi
                push    edi
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     esi, $$PROSEGPTR32
                movzx   ebx, word ptr [ebx]
                dec     ebx
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks
                push    eax
                lea     eax, [esp+1Ch+var_8 + FS_FIXES_STACK]
                call    checkForUnwindAtReturn
                pop     eax
                sub     dword ptr [edi], 8
                add     edi, [edi]
                mov     eax, [edi+8]
                mov     [esp+18h+var_4 + FS_FIXES_STACK], eax
                mov     eax, [esi].proseg_tidTrigger
                dec     dword ptr [eax+ebx*4]
                shl     ebx, 10h
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_IDT]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_IDT]
endif
ifdef FS_FIXES_3
                pop     fs
endif
                pop     edi
                pop     esi
                pop     ebx
                pop     eax
                popfd
                retn
$$PROFTRIGRET32 endp ; sp = -4

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public _ProfileHookVAOptlink32
_ProfileHookVAOptlink32 proc near
                retn
_ProfileHookVAOptlink32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public $$PROFEYEC32
$$PROFEYEC32    proc near

var_14          = dword ptr -14h
var_10          = dword ptr -10h
arg_0           = dword ptr  4
;arg_4          = byte ptr  8
arg_4           = dword ptr  8

                pushfd
                push    ebx
                push    esi
                push    edx
                push    eax
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jnz     short loc_6_2F4
                nop
                nop
                nop
                nop
                mov     eax, alreadyTriedInit
                or      eax, eax
                jnz     loc_6_40B
                call    DoInit
                mov     alreadyTriedInit, 1
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      loc_6_40B

loc_6_2F4:                              ; CODE XREF: $$PROFEYEC32+Dj
                test    dword ptr [esi].proseg_flags, 10h
                jnz     loc_6_40B
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi].proseg_maxTid
                jnb     loc_6_40B
                mov     eax, [esi].proseg_tidTrigger
                cmp     dword ptr [eax+ebx*4], 0
                jz      loc_6_40B
                mov     eax, [esi].proseg_tidDecNest
                dec     dword ptr [eax+ebx*4]
                js      loc_6_412
                push    edi
                lea     eax, [esp+18h + FS_FIXES_STACK]
                call    verifyStackFrame
                or      eax, eax
                jnz     loc_6_411
                mov     eax, offset $$PROFRET32
                xchg    eax, [esp+18h+arg_0 + FS_FIXES_STACK]
                cmp     eax, offset $$PROFRET32
                jnz     short loc_6_36A
                mov     eax, 0FFFFFFFDh
                call    PERF
                inc     dword ptr [esi._counterArray+14h]
                jmp     loc_6_411
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_36A:                              ; CODE XREF: $$PROFEYEC32+9Cj
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks
                push    eax
                lea     eax, [esp+1Ch + FS_FIXES_STACK]
                call    checkForUnwindAtCall
                pop     eax
                add     dword ptr [edi], 8
                add     edi, [edi]
                mov     [edi], eax
                lea     eax, [esp+18h + FS_FIXES_STACK]
                mov     [edi+4], eax
                mov     eax, [eax]
                push    dword ptr [edi]
                test    dword ptr [esi].proseg_flags, 20h
                jz      short loc_6_3AF
                nop
                nop
                nop
                nop
                shl     ebx, 10h
                mov     bl, 1
                mov     edx, [esp+1Ch+var_10 + FS_FIXES_STACK]
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif
                jmp     short loc_6_3B7
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
                ; align 4 ??
                nop
                nop
                nop
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_3AF:                              ; CODE XREF: $$PROFEYEC32+E1j
                shl     ebx, 10h
                mov     bl, 40h ; '@'
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif

loc_6_3B7:                              ; CODE XREF: $$PROFEYEC32+F3j
                pop     ebx
                mov     ebx, [ebx]
                cmp     bl, 0A9h ; 'Љ'
                jnz     short loc_6_40A
                xor     bl, bl
                lea     edi, [esp+18h+arg_4 + FS_FIXES_STACK]
                mov     eax, [esp+18h+var_14 + FS_FIXES_STACK]
                mov     esi, offset jmpback_1

loc_6_3CE:                              ; CODE XREF: $$PROFEYEC32+124j
                                        ; $$PROFEYEC32+12Fj ...
                shl     ebx, 1
                jb      short loc_6_3DD
                add     edi, 4
                shl     ebx, 1
                jb      short loc_6_3EF
                jz      short loc_6_40A
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_3DD:                              ; CODE XREF: $$PROFEYEC32+119j
                shl     ebx, 1
                jb      short loc_6_3E8
                fstp    qword ptr [edi]
                add     edi, 8
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_3E8:                              ; CODE XREF: $$PROFEYEC32+128j
                fstp    tbyte ptr [edi]
                add     edi, 10h
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_3EF:                              ; CODE XREF: $$PROFEYEC32+120j
                jmp     esi
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
jmpback_1:
                mov     esi, offset jmpback_2
                mov     [edi-4], eax
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
jmpback_2:
                mov     esi, offset jmpback_3
                mov     [edi-4], edx
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
jmpback_3:
                mov     [edi-4], ecx
                jmp     short loc_6_3CE
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_40A:                              ; CODE XREF: $$PROFEYEC32+106j
                                        ; $$PROFEYEC32+122j
                pop     edi

loc_6_40B:                              ; CODE XREF: $$PROFEYEC32+1Aj
                                        ; $$PROFEYEC32+37j ...
ifdef FS_FIXES_3
                pop     fs
endif
                pop     eax
                pop     edx
                pop     esi
                pop     ebx
                popfd
                retn
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_411:                              ; CODE XREF: $$PROFEYEC32+88j
                                        ; $$PROFEYEC32+AEj
                pop     edi

loc_6_412:                              ; CODE XREF: $$PROFEYEC32+76j
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                jmp     short loc_6_40B
$$PROFEYEC32    endp

; Type idx: 513
; Type idx: 183

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public PERF
        public $$PROFUSER32
$$PROFUSER32 proc near
PERF label near:                                ; CODE XREF: $$PROFEYEC32+A3p
                                        ; PerfStart+2Bp ...
                retn                    ; $$PROFUSER32

; Type idx: 183

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public PROFITCAL
PROFITCAL       label near ; CODE XREF: uCal+17p uCal+1Cp
                push    ebx
                push    esi
                push    ecx
                push    edx
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jnz     short loc_6_45A
                nop
                nop
                nop
                nop
                mov     eax, alreadyTriedInit
                or      eax, eax
                jnz     short loc_6_494
                nop
                nop
                nop
                nop
                call    DoInit
                mov     alreadyTriedInit, 1
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_494
                nop
                nop
                nop
                nop

loc_6_45A:                              ; CODE XREF: PROFITCAL+Cj
                test    dword ptr [esi].proseg_flags, 10h
                jnz     short loc_6_494
                nop
                nop
                nop
                nop
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi].proseg_maxTid
                jnb     short loc_6_494
                mov     edx, [esi].proseg_tidDecNest
                mov     edx, [edx+ebx*4]
                mov     ecx, [esi].proseg_tidMaxNest
                cmp     edx, [ecx+ebx*4]
                jz      short loc_6_494
                push    edi
                shl     ebx, 10h
                mov     bl, 20h ; ' '
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif
                pop     edi

loc_6_494:                              ; CODE XREF: PROFITCAL+19j
                                        ; PROFITCAL+36j ...
ifdef FS_FIXES_3
                pop     fs
endif
                pop     edx
                pop     ecx
                pop     esi
                pop     ebx
                retn
$$PROFUSER32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public PerfStart
PerfStart       proc near
                push    eax
                push    esi
                push    ecx
                push    edx
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_4C9
                nop
                nop
                nop
                nop
                xor     ecx, ecx
                xor     edx, edx
                mov     dx, [esi].proseg_flags
                mov     cx, dx
                and     dx, 10h
                xor     cx, dx
                mov     [esi].proseg_flags, cx
                mov     eax, 0FFFFFFFEh
                call    PERF

loc_6_4C9:                              ; CODE XREF: PerfStart+Cj
                pop     edx
                pop     ecx
                pop     esi
                pop     eax
                retn
PerfStart       endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public PerfStop
PerfStop        proc near
                push    eax
                push    esi
                push    ecx
                push    edx
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_4F8
                nop
                nop
                nop
                nop
                mov     eax, 0FFFFFFFFh
                call    PERF
                xor     ecx, ecx
                xor     edx, edx
                mov     cx, [esi].proseg_flags
                or      cx, 10h
                mov     [esi].proseg_flags, cx

loc_6_4F8:                              ; CODE XREF: PerfStop+Cj
                pop     edx
                pop     ecx
                pop     esi
                pop     eax
                retn
PerfStop        endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public setjmp
setjmp          proc near
;arg_0          = byte ptr  4
arg_0           = dword ptr  4
                mov     ecx, [esp+0]

; Type idx: 183

                public $$PROFSETJMP
$$PROFSETJMP label near

                call    _ProfileHook32
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                mov     [eax+4], ecx
                mov     [eax+0Ch], ebx
                mov     [eax+14h], esi
                mov     [eax+10h], edi
                mov     [eax], ebp
                lea     edx, [esp+arg_0]
                mov     [eax+8], edx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     edx, fs:tib_s.tib_pexchain
                pop     fs
else
                mov     edx, fs:tib_s.tib_pexchain
endif
                mov     [eax+18h], edx
                fstcw   word ptr [eax+1Ch]
                xor     eax, eax
ifdef FS_FIXES_3
                pop     fs
endif
                retn
setjmp          endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public longjmp
longjmp         proc near
                call    _ProfileHook32
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    eax
                push    edx
ifdef FS_FIXES_1
                push    fs
endif
                push    0
                push    offset unwind_callback
                push    dword ptr [eax+18h]
                call    DosUnwindException
                mov     edx, 0FCCCh
                jmp     CrashAndBurn

; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
unwind_callback:
                add     esp, 0Ch
ifdef FS_FIXES_1
                pop     fs
endif
                pop     edx
                pop     eax
ifdef FS_FIXES_1
                mov     ecx, $$PROSEGPTR32
                jecxz   @@jump
                jmp     @@ok
@@jump:
                jmp     loc_6_5C2
@@ok:
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ecx, $$PROSEGPTR32
                jecxz   loc_6_5C2
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     esi, ecx
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi].proseg_maxTid
                jnb     short loc_6_5C2
                push    eax
                push    edx
                mov     eax, [eax+8]
                xor     ecx, ecx
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks

loc_6_57C:                              ; CODE XREF: CODE32:000005B0j
                mov     edx, [edi]
                or      edx, edx
                jz      short loc_6_5B2
                cmp     eax, [edi+edx+4]
                jbe     short loc_6_5B2
                mov     edx, [edi+edx+4]
                mov     edx, [edx+4]
                cmp     edx, offset $$PROFTRIGRET32
                jnz     short loc_6_5A2
                push    eax
                mov     eax, [esi].proseg_tidTrigger
                dec     dword ptr [eax+ebx*4]
                pop     eax

loc_6_5A2:                              ; CODE XREF: CODE32:00000595j
                push    eax
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                pop     eax
                sub     dword ptr [edi], 8
                loop    loc_6_57C

loc_6_5B2:                              ; CODE XREF: CODE32:00000580j
                                        ; CODE32:00000586j
                neg     ecx
                jz      short bailout2
                mov     eax, ecx
                shl     ebx, 10h
                mov     bl, 10h
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif
                pop     edx
                pop     eax

loc_6_5C2:                              ; CODE XREF: CODE32:00000556j
                                        ; CODE32:00000568j
                xchg    eax, edx
                finit
                wait
                fldcw   word ptr [edx+1Ch]
ifdef FS_FIXES_3
                pop     fs
endif
                mov     ebx, [edx+0Ch]
                mov     esi, [edx+14h]
                mov     edi, [edx+10h]
                mov     ebp, [edx]
                mov     esp, [edx+8]
                or      eax, eax
                jnz     short bailout1
                inc     eax

bailout1:                               ; CODE XREF: CODE32:000005B4j
                                        ; CODE32:000005DAj
                jmp     dword ptr [edx+4]

bailout2:
ifdef FS_FIXES_3
                pop     fs
endif
                jmp     dword ptr [edx+4]

longjmp         endp
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
ExceptionThrownString   db 'C++ exception thrown here:',0
; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public _ProfileThrow32
_ProfileThrow32 proc near
                push    eax
                mov     eax, offset ExceptionThrownString
                call    PERF
                pop     eax
                jmp     PERF
_ProfileThrow32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public _ProfileUnwind32
_ProfileUnwind32 proc near
                push    ebx
                push    esi
                push    edi
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif

                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_67A
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                movzx   ebx, word ptr [ebx]
                dec     ebx
                cmp     ebx, [esi].proseg_maxTid
                jnb     short loc_6_67A
                xor     ecx, ecx
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks

loc_6_636:                              ; CODE XREF: _ProfileUnwind32+5Ej
                mov     edx, [edi]
                or      edx, edx
                jz      short loc_6_66C
                cmp     eax, [edi+edx+4]
                jbe     short loc_6_66C
                mov     edx, [edi+edx+4]
                mov     edx, [edx+4]
                cmp     edx, offset $$PROFTRIGRET32
                jnz     short loc_6_65C
                push    eax
                mov     eax, [esi].proseg_tidTrigger
                dec     dword ptr [eax+ebx*4]
                pop     eax

loc_6_65C:                              ; CODE XREF: _ProfileUnwind32+43j
                push    eax
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax+ebx*4]
                pop     eax
                sub     dword ptr [edi], 8
                loop    loc_6_636

loc_6_66C:                              ; CODE XREF: _ProfileUnwind32+2Ej
                                        ; _ProfileUnwind32+34j
                neg     ecx
                jz      short loc_6_67A
                mov     eax, ecx
                shl     ebx, 10h
                mov     bl, 10h
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif

loc_6_67A:                              ; CODE XREF: _ProfileUnwind32+Bj
                                        ; _ProfileUnwind32+1Bj ...
ifdef FS_FIXES_3
                pop     fs
endif
                pop     edi
                pop     esi
                pop     ebx
                retn
_ProfileUnwind32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public eCal
eCal            proc near
                push    esi
                mov     esi, $$PROSEGPTR32
                mov     ecx, [esi].proseg_tidDecNest
                dec     dword ptr [ecx]
                mov     ecx, 8

loc_6_692:                              ; CODE XREF: CODE32:000006A7j
                push    offset eCal_arg2
                push    offset eCal_arg1
                push    offset $$PROFCALL32
                jmp     $$PROFCALL32

; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
eCal_arg2:
                retn
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
eCal_arg1:
                loop    loc_6_692
                push    eax
                mov     eax, [esi].proseg_tidDecNest
                inc     dword ptr [eax]
                pop     eax
                pop     esi
                retn
eCal            endp
; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public uCal
uCal            proc near
                mov     $$PROSEGPTR32, eax
                call    $$PROFCALL32
                push    esi
                mov     esi, $$PROSEGPTR32
                mov     ecx, 8

loc_6_6CB:                              ; CODE XREF: uCal+22j
                push    ecx
                call    PROFITCAL
                call    PROFITCAL
                pop     ecx
                loop    loc_6_6CB
                pop     esi
                retn
uCal            endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public CrashAndBurn
CrashAndBurn    proc near               ; CODE XREF: longjmp+1Bj DoInit+1Fj ...
                mov     [esi].proseg_rc, edx
                mov     [esi].proseg_rcSys, eax
                push    eax
                push    1
                call    DosExit
CrashAndBurn    endp

; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
initName    db 'INIT_PROSEG',0      ; DATA XREF: DoInit+2Eo
logDllName  db 'CPPPAN40',0         ; DATA XREF: DoInit+7o
; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл

public  DoInit
DoInit  proc near               ; CODE XREF: $$PROFCALL32+20p
                                        ; $$PROFTRIG32+20p ...
                pushad
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                xor     ebx, ebx
                push    ebx
                mov     eax, esp
ifdef FS_FIXES_1
                push    fs
endif
                push    eax
                lea     eax, logDllName ; "CPPPAN40"
                push    eax
                push    ebx
                push    ebx
                call    DosLoadModule
                add     esp, 10h
ifdef FS_FIXES_1
                pop     fs
endif
                mov     edx, 0FAD8h
                or      eax, eax
                jnz     short CrashAndBurn
                pop     edx
                mov     ecx, 7
ifdef FS_FIXES_1
                push    fs
endif

                lea     edi, initAdr
                push    edi
                lea     ebx, initName ; "INIT_PROSEG"
                push    ebx
                push    0
                push    edx
                call    DosQueryProcAddr
                add     esp, 10h
ifdef FS_FIXES_1
                pop     fs
endif
                mov     edx, 0FB3Ch
                or      eax, eax
                jnz     short CrashAndBurn
                xor     ebx, ebx

                push    ebx
                push    ebx
                mov     eax, esp
                mov     edx, offset eCal
                mov     ecx, offset uCal
ifdef FS_FIXES_1
                push    fs
endif
                lea     esi, $$PROSEGPTR32
                push    esi
                sub     esp, 0Ch
                call    initAdr
                add     esp, 10h
ifdef FS_FIXES_1
                pop     fs
endif
                or      eax, eax
                pop     edx
                pop     eax
                jnz     CrashAndBurn
ifdef FS_FIXES_3
                pop     fs
endif
                popad
                mov     esi, $$PROSEGPTR32
                retn
DoInit  endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public checkForUnwindAtCall
checkForUnwindAtCall proc near          ; CODE XREF: $$PROFCALL32+B0p
                                        ; $$PROFTRIG32+ACp ...
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                test    word ptr [esi].proseg_flags, 40h
                jz      short locret_6_7D0
                test    word ptr [esi].proseg_flags, 80h
                jz      short locret_6_7D0
                push    edx
                push    ecx
                mov     edx, edi
                add     edx, [edx]
                xor     ecx, ecx

loc_6_792:                              ; CODE XREF: checkForUnwindAtCall+25j
                cmp     edx, edi
                jbe     short loc_6_7AB
                cmp     [edx+4], eax
                ja      short loc_6_7AB
                sub     edx, 8
                sub     dword ptr [edi], 8
                loop    loc_6_792
                xor     eax, eax
                mov     dword ptr [eax], 0

loc_6_7AB:                              ; CODE XREF: checkForUnwindAtCall+18j
                                        ; checkForUnwindAtCall+1Dj
                neg     ecx
                jz      short loc_6_7CE
                add     dword ptr [esi+ebx*4+158h], ecx
                mov     eax, 0FFFFFFFCh
                call    PERF
                push    ebx
                push    edi
                shl     ebx, 10h
                mov     bl, 10h
                mov     eax, ecx
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif
                pop     edi
                pop     ebx

loc_6_7CE:                              ; CODE XREF: checkForUnwindAtCall+31j
                pop     ecx
                pop     edx

locret_6_7D0:                           ; CODE XREF: checkForUnwindAtCall+5j
                                        ; checkForUnwindAtCall+Cj
ifdef FS_FIXES_3
                pop     fs
endif
                retn
checkForUnwindAtCall endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public checkForUnwindAtReturn
checkForUnwindAtReturn proc near        ; CODE XREF: $$PROFRET32+33p
                                        ; $$PROFTRIGRET32+33p
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                test    word ptr [esi].proseg_flags, 40h
                jz      short locret_6_82F
                test    word ptr [esi].proseg_flags, 80h
                jz      short locret_6_82F
                cmp     dword ptr [edi], 10h
                jb      short locret_6_82F
                push    edx
                push    ecx
                mov     edx, edi
                add     edx, [edx]
                xor     ecx, ecx

loc_6_7EC:                              ; CODE XREF: checkForUnwindAtReturn+2Fj
                cmp     [edx+4], eax
                jnb     short loc_6_80A
                sub     edx, 8
                cmp     edx, edi
                jbe     short loc_6_80A
                cmp     [edx+4], eax
                ja      short loc_6_80A
                sub     dword ptr [edi], 8
                loop    loc_6_7EC
                xor     eax, eax
                mov     dword ptr [eax], 0

loc_6_80A:                              ; CODE XREF: checkForUnwindAtReturn+1Ej
                                        ; checkForUnwindAtReturn+25j ...
                neg     ecx
                jz      short loc_6_82D
                add     dword ptr [esi+ebx*4+158h], ecx
                mov     eax, 0FFFFFFFBh
                call    PERF
                push    ebx
                push    edi
                shl     ebx, 10h
                mov     bl, 10h
                mov     eax, ecx
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID4T]
endif
                pop     edi
                pop     ebx

loc_6_82D:                              ; CODE XREF: checkForUnwindAtReturn+3Bj
                pop     ecx
                pop     edx

locret_6_82F:                           ; CODE XREF: checkForUnwindAtReturn+5j
                                        ; checkForUnwindAtReturn+Cj ...
ifdef FS_FIXES_3
                pop     fs
endif
                retn
checkForUnwindAtReturn endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public verifyStackFrame
verifyStackFrame proc near              ; CODE XREF: $$PROFCALL32+8Ep
                                        ; $$PROFTRIG32+7Ap ...
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    edi
                test    word ptr [esi].proseg_flags, 40h
                jz      short loc_6_85F
                test    word ptr [esi].proseg_flags, 80h
                jz      short loc_6_85F
                mov     edi, ebx
                shl     edi, 0Ch
                add     edi, [esi].proseg_retStacks
                cmp     dword ptr [edi], 0
                jz      short loc_6_85F
                nop
                nop
                nop
                nop
                cmp     eax, [edi+0Ch]
                ja      short loc_6_863
                add     edi, [edi]
                cmp     eax, [edi+4]
                jz      short loc_6_86A

loc_6_85F:                              ; CODE XREF: verifyStackFrame+6j
                                        ; verifyStackFrame+Dj ...
                xor     eax, eax
                jmp     short loc_6_879
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_863:                              ; CODE XREF: verifyStackFrame+26j
                mov     eax, 0FFFFFFF9h
                jmp     short loc_6_86F
; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ

loc_6_86A:                              ; CODE XREF: verifyStackFrame+2Dj
                mov     eax, 0FFFFFFFDh

loc_6_86F:                              ; CODE XREF: verifyStackFrame+38j
                call    PERF
                mov     eax, 1

loc_6_879:                              ; CODE XREF: verifyStackFrame+31j
                pop     edi
ifdef FS_FIXES_3
                pop     fs
endif
                retn
verifyStackFrame endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл

; Attributes: bp-based frame

                public _ProfileEnterParagraph32
_ProfileEnterParagraph32 proc near
                push    ebp
                mov     ebp, esp
                pushfd
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    esi
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      loc_6_8EE
                test    word ptr [esi].proseg_flags, 2
                jz      short loc_6_8EE
                test    word ptr [esi].proseg_flags, 10h
                jnz     short loc_6_8EE
                push    edi
                push    ebx
                push    ecx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     ebx, [ebx]
                cmp     ebx, [esi].proseg_maxTid
                ja      short loc_6_8EB
                dec     ebx
                mov     eax, [esi].proseg_tidTrigger
                cmp     dword ptr [eax+ebx*4], 0
                jz      short loc_6_8EB
                mov     edi, ebx
                shl     ebx, 10h
                mov     eax, [ebp+4]
                push    edx
                mov     ecx, [esi].conditionalExit
                mov     edx, [ecx+edi*4]
                cmp     edx, 0FFFFFFFFh
                jz      short loc_6_8E5
                mov     bl, 5
                push    eax
                push    ebx
                push    edi
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif
                pop     edi
                pop     ebx
                pop     eax
                mov     ecx, [esi].conditionalExit
                mov     dword ptr [ecx+edi*4], 0FFFFFFFFh

loc_6_8E5:                              ; CODE XREF: _ProfileEnterParagraph32+50j
                mov     bl, 4
                pop     edx
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif

loc_6_8EB:                              ; CODE XREF: _ProfileEnterParagraph32+2Cj
                                        ; _ProfileEnterParagraph32+39j
                pop     ecx
                pop     ebx
                pop     edi

loc_6_8EE:                              ; CODE XREF: _ProfileEnterParagraph32+Dj
                                        ; _ProfileEnterParagraph32+14j ...
                pop     esi
ifdef FS_FIXES_3
                pop     fs
endif
                popfd
                pop     ebp
                retn
_ProfileEnterParagraph32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл

; Attributes: bp-based frame

                public _ProfileExitParagraph32
_ProfileExitParagraph32 proc near
                push    ebp
                mov     ebp, esp
                pushfd
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    esi
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_939
                test    word ptr [esi].proseg_flags, 2
                jz      short loc_6_939
                test    word ptr [esi].proseg_flags, 10h
                jnz     short loc_6_939
                push    ebx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     ebx, [ebx]
                cmp     ebx, [esi].proseg_maxTid
                ja      short loc_6_938
                dec     ebx
                mov     edx, [esi].proseg_tidTrigger
                cmp     dword ptr [edx+ebx*4], 0
                jz      short loc_6_938
                mov     edx, eax
                mov     eax, [ebp+4]
                shl     ebx, 10h
                mov     bl, 5
ifdef FS_FIXES_2
                push    fs
                push    Dos32TIB
                pop     fs
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
                pop     fs
else
                call    dword ptr [esi.proseg_pfnINIT_ID44T]
endif

loc_6_938:                              ; CODE XREF: _ProfileExitParagraph32+2Aj
                                        ; _ProfileExitParagraph32+37j
                pop     ebx

loc_6_939:                              ; CODE XREF: _ProfileExitParagraph32+Dj
                                        ; _ProfileExitParagraph32+14j ...
                pop     esi
ifdef FS_FIXES_3
                pop     fs
endif
                popfd
                pop     ebp
                retn
_ProfileExitParagraph32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public _ProfileConditionalExit32
_ProfileConditionalExit32 proc near
                pushfd
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    esi
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_973
                test    word ptr [esi].proseg_flags, 2
                jz      short loc_6_973
                test    word ptr [esi].proseg_flags, 10h
                jnz     short loc_6_973
                push    ebx
                push    ecx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     ebx, [ebx]
                cmp     ebx, [esi].proseg_maxTid
                ja      short loc_6_971
                dec     ebx
                mov     ecx, [esi].conditionalExit
                mov     [ecx+ebx*4], eax

loc_6_971:                              ; CODE XREF: _ProfileConditionalExit32+28j
                pop     ecx
                pop     ebx

loc_6_973:                              ; CODE XREF: _ProfileConditionalExit32+Aj
                                        ; _ProfileConditionalExit32+11j ...
                pop     esi
ifdef FS_FIXES_3
                pop     fs
endif
                popfd
                retn
_ProfileConditionalExit32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл


                public _ProfileCancelExit32
_ProfileCancelExit32 proc near
                pushfd
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    esi
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_9B0
                test    word ptr [esi].proseg_flags, 2
                jz      short loc_6_9B0
                test    word ptr [esi].proseg_flags, 10h
                jnz     short loc_6_9B0
                push    ebx
                push    ecx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     ebx, [ebx]
                cmp     ebx, [esi].proseg_maxTid
                ja      short loc_6_9AE
                dec     ebx
                mov     ecx, [esi].conditionalExit
                mov     dword ptr [ecx+ebx*4], 0FFFFFFFFh

loc_6_9AE:                              ; CODE XREF: _ProfileCancelExit32+28j
                pop     ecx
                pop     ebx

loc_6_9B0:                              ; CODE XREF: _ProfileCancelExit32+Aj
                                        ; _ProfileCancelExit32+11j ...
                pop     esi
ifdef FS_FIXES_3
                pop     fs
endif
                popfd
                retn
_ProfileCancelExit32 endp

; Type idx: 513

; ллллллллллллллл S U B R O U T I N E ллллллллллллллллллллллллллллллллллллллл

; Attributes: bp-based frame

                public _ProfileNameString32
_ProfileNameString32 proc near
                push    ebp
                mov     ebp, esp
                pushfd
ifdef FS_FIXES_3
                push    fs
                push    Dos32TIB
                pop     fs
endif
                push    esi
                mov     esi, $$PROSEGPTR32
                or      esi, esi
                jz      short loc_6_A11
                test    word ptr [esi].proseg_flags, 2
                jz      short loc_6_A11
                test    word ptr [esi].proseg_flags, 10h
                jnz     short loc_6_A11
                push    ebx
                push    ecx
ifdef FS_FIXES_1
                push    fs
                push    Dos32TIB
                pop     fs
                mov     ebx, fs:tib_s.tib_ptib2
                pop     fs
else
                mov     ebx, fs:tib_s.tib_ptib2
endif
                mov     ebx, [ebx]
                cmp     ebx, [esi].proseg_maxTid
                ja      short loc_6_A0F
                dec     ebx
                mov     ecx, [esi].proseg_tidTrigger
                cmp     dword ptr [ecx+ebx*4], 0
                jz      short loc_6_A0F
                mov     ecx, [ebp+4]
                mov     [esi._parm   ], ecx
                mov     [esi._parm+4h], eax
                mov     [esi._parm+8h], edx
                mov     dword ptr [esi]._command, 2
                int     3               ; Trap to Debugger
                wait
                int     3               ; Trap to Debugger

loc_6_A0F:                              ; CODE XREF: _ProfileNameString32+2Bj
                                        ; _ProfileNameString32+38j
                pop     ecx
                pop     ebx

loc_6_A11:                              ; CODE XREF: _ProfileNameString32+Dj
                                        ; _ProfileNameString32+14j ...
                pop     esi
ifdef FS_FIXES_3
                pop     fs
endif
                popfd
                pop     ebp
                retn
_ProfileNameString32 endp

; ФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФФ
; Type idx: 520

                public _ProfileVersionId32
_ProfileVersionId32 TVersionId <4, 0, 2, 178h>
CODE32          ends

                end
