; $Id: arith64.asm,v 1.3 1999-12-20 11:47:00 sandervl Exp $

;/*
; * 64-bit integer apis
; *
; * Copyright 1999 Patrick Haller
; *
; *
; * Project Odin Software License can be found in LICENSE.TXT
; *
; */
.586P
               NAME    arith64

CODE32         SEGMENT DWORD PUBLIC USE32 'CODE'
               align 4

; ----------------------------------------------------------------------------

       public  _RtlLargeIntegerDivide@20
       public  _RtlLargeIntegerAdd@16
       public  _RtlEnlargedIntegerMultiply@8
       public  _RtlEnlargedUnsignedMultiply@8
       public  _RtlEnlargedUnsignedDivide@16
       public  _RtlExtendedLargeIntegerDivide@16
       public  _RtlExtendedMagicDivide@20
       public  _RtlExtendedIntegerMultiply@12
       public  _RtlLargeIntegerShiftLeft@12
       public  _RtlLargeIntegerShiftRight@12
       public  _RtlLargeIntegerArithmeticShift@12
       public  _RtlLargeIntegerNegate@8
       public  _RtlLargeIntegerSubtract@16
       public  _RtlConvertLongToLargeInteger@4
       public  _RtlConvertUlongToLargeInteger@4

; ----------------------------------------------------------------------------


  extrn         RtlRaiseStatus : near

; ----------------------------------------------------------------------------


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerDivide
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.462
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerDivide@20 proc near

var_10         = dword ptr -10h
var_C          = dword ptr -0Ch
var_8          = dword ptr -8
var_4          = dword ptr -4
arg_0          = dword ptr  8
arg_4          = dword ptr  0Ch
arg_8          = dword ptr  10h
arg_C          = dword ptr  14h
arg_10         = dword ptr  18h

               push    ebp
               xor     eax, eax
               mov     ebp, esp
               sub     esp, 10h
               push    esi
               mov     [ebp+var_8], eax
               push    edi
               mov     esi, 40h
               mov     edi, [ebp+arg_C]
               mov     [ebp+var_4], eax
               mov     eax, [ebp+arg_8]
               or      eax, edi
               jnz     short _rtl_lrgintdiv_1
               push    0C0000094h
               call    RtlRaiseStatus

_rtl_lrgintdiv_1:
               mov     eax, [ebp+arg_0]
               mov     ecx, [ebp+arg_4]
               mov     [ebp+var_10], eax
               mov     [ebp+var_C], ecx

_rtl_lrgintdiv_2:
               mov     eax, [ebp+var_8]
               mov     ecx, [ebp+var_4]
               shr     eax, 1Fh
               add     ecx, ecx
               or      eax, ecx
               mov     ecx, [ebp+var_8]
               add     ecx, ecx
               mov     [ebp+var_4], eax
               mov     eax, [ebp+var_C]
               shr     eax, 1Fh
               or      eax, ecx
               mov     ecx, [ebp+var_C]
               add     ecx, ecx
               mov     [ebp+var_8], eax
               mov     eax, [ebp+var_10]
               shl     [ebp+var_10], 1
               shr     eax, 1Fh
               or      eax, ecx
               cmp     [ebp+var_4], edi
               mov     [ebp+var_C], eax
               ja      short _rtl_lrgintdiv_3
               mov     eax, [ebp+arg_C]
               cmp     [ebp+var_4], eax
               jnz     short _rtl_lrgintdiv_5
               mov     eax, [ebp+arg_8]
               cmp     [ebp+var_8], eax
               jb      short _rtl_lrgintdiv_5

_rtl_lrgintdiv_3:
               mov     eax, [ebp+arg_C]
               mov     ecx, [ebp+arg_8]
               or      byte ptr [ebp+var_10], 1
               sub     [ebp+var_4], eax
               cmp     [ebp+var_8], ecx
               jnb     short _rtl_lrgintdiv_4
               dec     [ebp+var_4]

_rtl_lrgintdiv_4:
               mov     eax, [ebp+arg_8]
               sub     [ebp+var_8], eax

_rtl_lrgintdiv_5:
               dec     esi
               jnz     short _rtl_lrgintdiv_2
               mov     eax, [ebp+arg_10]
               test    eax, eax
               jz      short _rtl_lrgintdiv_6
               mov     ecx, [ebp+var_8]
               mov     edx, [ebp+var_4]
               mov     [eax], ecx
               mov     [eax+4], edx

_rtl_lrgintdiv_6:
               mov     eax, [ebp+var_10]
               mov     edx, [ebp+var_C]
               pop     edi
               pop     esi
               mov     esp, ebp
               pop     ebp
               retn    14h
_RtlLargeIntegerDivide@20 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerAdd
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.460
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerAdd@16 proc        near

arg_0          = dword ptr  4
arg_4          = dword ptr  8
arg_8          = dword ptr  0Ch
arg_C          = dword ptr  10h

               mov     eax, [esp+arg_0]
               add     eax, [esp+arg_8]
               mov     edx, [esp+arg_4]
               adc     edx, [esp+arg_C]
               retn    10h
_RtlLargeIntegerAdd@16 endp


; ----------------------------------------------------------------------------
; Name      : RtlEnlargedIntegerMultiply
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.371
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlEnlargedIntegerMultiply@8 proc near

arg_0           = dword ptr  4
arg_4           = dword ptr  8

                mov     eax, [esp+arg_0]
                imul    [esp+arg_4]
                retn    8
_RtlEnlargedIntegerMultiply@8 endp


; ----------------------------------------------------------------------------
; Name      : RtlEnlargedUnsignedMultiply
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.373
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlEnlargedUnsignedMultiply@8 proc near

arg_0           = dword ptr  4
arg_4           = dword ptr  8
                mov     eax, [esp+arg_0]
                mul     [esp+arg_4]
                retn    8
_RtlEnlargedUnsignedMultiply@8 endp


; ----------------------------------------------------------------------------
; Name      : RtlEnlargedUnsignedDivide
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.372
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlEnlargedUnsignedDivide@16 proc near

arg_0           = dword ptr  4
arg_4           = dword ptr  8
arg_8           = dword ptr  0Ch
arg_C           = dword ptr  10h

                mov     eax, [esp+arg_0]
                mov     edx, [esp+arg_4]
                mov     ecx, [esp+arg_C]
                div     [esp+arg_8]
                or      ecx, ecx
                jnz     short _rtl_enlunsdiv_1
                retn    10h

_rtl_enlunsdiv_1:
                mov     [ecx], edx
                retn    10h
_RtlEnlargedUnsignedDivide@16 endp


; ----------------------------------------------------------------------------
; Name      : RtlExtendedLargeIntegerDivide
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.390
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlExtendedLargeIntegerDivide@16 proc near

arg_0           = dword ptr  10h
arg_4           = dword ptr  14h
arg_8           = dword ptr  18h
arg_C           = dword ptr  1Ch

                push    esi
                push    edi
                push    ebx
                mov     eax, [esp+arg_0]
                mov     edx, [esp+arg_4]
                mov     ebx, [esp+arg_8]
                or      ebx, ebx
                jz      short _rtl_extlrgintdiv_2
                push    ebp
                mov     ecx, 40h
                xor     esi, esi
                nop

_rtl_extlrgintdiv_1:
                shl     eax, 1
                rcl     edx, 1
                rcl     esi, 1
                sbb     edi, edi
                cmp     esi, ebx
                cmc
                sbb     ebp, ebp
                or      edi, ebp
                sub     eax, edi
                and     edi, ebx
                sub     esi, edi
                dec     ecx
                jnz     short _rtl_extlrgintdiv_1
                pop     ebp
                pop     ebx
                pop     edi
                mov     ecx, [esp+0FFFFFFF8h+arg_C]
                or      ecx, ecx
                jnz     short _rtl_extlrgintdiv_3
                pop     esi
                retn    10h

_rtl_extlrgintdiv_3:
                mov     [ecx], esi
                pop     esi
                retn    10h

_rtl_extlrgintdiv_2:
                push    0C0000094h
                call    RtlRaiseStatus
                pop     ebx
                pop     edi
                pop     esi
                retn    10h
_RtlExtendedLargeIntegerDivide@16 endp



; ----------------------------------------------------------------------------
; Name      : RtlExtendedMagicDivide
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.391
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlExtendedMagicDivide@20 proc near

var_C          = dword ptr -0Ch
var_8          = dword ptr -8
var_4          = dword ptr -4
arg_0          = dword ptr  8
arg_4          = dword ptr  0Ch
arg_8          = dword ptr  10h
arg_C          = dword ptr  14h
arg_10         = byte ptr  18h

               push    ebp
               mov     ebp, esp
               sub     esp, 0Ch
               push    esi
               mov     esi, [ebp+arg_4]
               test    esi, 80000000h
               jz      short _rtl_extmagicdiv_1
               neg     [ebp+arg_4]
               neg     [ebp+arg_0]
               sbb     [ebp+arg_4], 0

_rtl_extmagicdiv_1:
               mov     eax, [ebp+arg_8]
               mul     [ebp+arg_0]
               mov     [ebp+var_4], edx
               mov     eax, [ebp+arg_8]
               mul     [ebp+arg_4]
               mov     [ebp+var_8], eax
               mov     [ebp+var_C], edx
               mov     eax, [ebp+arg_C]
               mul     [ebp+arg_0]
               xor     ecx, ecx
               add     eax, [ebp+var_4]
               adc     ecx, 0
               add     eax, [ebp+var_8]
               adc     ecx, 0
               mov     [ebp+var_4], edx
               mov     eax, [ebp+arg_C]
               mul     [ebp+arg_4]
               add     eax, [ebp+var_4]
               adc     edx, 0
               add     eax, [ebp+var_C]
               adc     edx, 0
               add     eax, ecx
               adc     edx, 0
               mov     cl, [ebp+arg_10]

_rtl_extmagicdiv_3:
               cmp     cl, 1Fh
               jbe     short _rtl_extmagicdiv_2
               sub     cl, 1Fh
               shrd    eax, edx, 1Fh
               shr     edx, 1Fh
               jmp     short _rtl_extmagicdiv_3

_rtl_extmagicdiv_2:
               shrd    eax, edx, cl
               shr     edx, cl
               test    esi, 80000000h
               jz      short _rtl_extmagicdiv_4
               neg     edx
               neg     eax
               sbb     edx, 0

_rtl_extmagicdiv_4:
               pop     esi
               mov     esp, ebp
               pop     ebp
               retn    14h
_RtlExtendedMagicDivide@20 endp


; ----------------------------------------------------------------------------
; Name      : RtlExtendedIntegerMultiply
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.389
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlExtendedIntegerMultiply@12 proc        near

var_8          = dword ptr -8
arg_0          = dword ptr  8
arg_4          = dword ptr  0Ch
arg_8          = dword ptr  10h

               push    ebp
               mov     ebp, esp
               push    esi
               mov     esi, [ebp+arg_8]
               xor     esi, [ebp+arg_4]
               test    [ebp+arg_4], 80000000h
               jz      short _rtl_extintmply_1
               neg     [ebp+arg_4]
               neg     [ebp+arg_0]
               sbb     [ebp+arg_4], 0

_rtl_extintmply_1:
               test    [ebp+arg_8], 80000000h
               jz      short _rtl_extintmply_2
               neg     [ebp+arg_8]

_rtl_extintmply_2:
               mov     eax, [ebp+arg_8]
               mul     [ebp+arg_0]
               push    edx
               mov     ecx, eax
               mov     eax, [ebp+arg_8]
               mul     [ebp+arg_4]
               add     eax, [esp+8+var_8]
               test    esi, 80000000h
               jz      short _rtl_extintmply_3
               neg     eax
               neg     ecx
               sbb     eax, 0

_rtl_extintmply_3:
               add     esp, 4
               pop     esi
               mov     edx, eax
               mov     eax, ecx
               pop     ebp
               retn    0Ch
_RtlExtendedIntegerMultiply@12 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerShiftLeft
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.464
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerShiftLeft@12 proc near

arg_0          = dword ptr  4
arg_4          = dword ptr  8
arg_8          = dword ptr  0Ch

               mov     ecx, [esp+arg_8]
               and     ecx, 3Fh
               cmp     ecx, 20h
               jnb     short _rtl_lrgintshl_1
               mov     eax, [esp+arg_0]
               mov     edx, [esp+arg_4]
               shld    edx, eax, cl
               shl     eax, cl
               retn    0Ch

_rtl_lrgintshl_1:
               mov     edx, [esp+arg_0]
               xor     eax, eax
               shl     edx, cl
               retn    0Ch
_RtlLargeIntegerShiftLeft@12 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerShiftRight
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.465
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerShiftRight@12 proc near

arg_0          = dword ptr  4
arg_4          = dword ptr  8
arg_8          = dword ptr  0Ch

               mov     ecx, [esp+arg_8]
               and     ecx, 3Fh
               cmp     ecx, 20h
               jnb     short _rtl_lrgintshr_1
               mov     eax, [esp+arg_0]
               mov     edx, [esp+arg_4]
               shrd    eax, edx, cl
               shr     edx, cl
               retn    0Ch

_rtl_lrgintshr_1:
               mov     eax, [esp+arg_4]
               xor     edx, edx
               shr     eax, cl
               retn    0Ch
_RtlLargeIntegerShiftRight@12 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerArithmeticShift
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.461
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------

_RtlLargeIntegerArithmeticShift@12 proc near

arg_0          = dword ptr  4
arg_4          = dword ptr  8
arg_8          = dword ptr  0Ch

               mov     ecx, [esp+arg_8]
               and     ecx, 3Fh
               cmp     ecx, 20h
               jb      short _rtl_lrgintarithshft_1
               mov     eax, [esp+arg_4]
               sar     eax, cl
               bt      eax, 1Fh
               sbb     edx, edx
               retn    0Ch

_rtl_lrgintarithshft_1:
               mov     eax, [esp+arg_0]
               mov     edx, [esp+arg_4]
               shrd    eax, edx, cl
               sar     edx, cl
               retn    0Ch
_RtlLargeIntegerArithmeticShift@12 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerNegate
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.463
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerNegate@8 proc near

arg_0          = dword ptr  4
arg_4          = dword ptr  8

               mov     eax, [esp+arg_0]
               mov     edx, [esp+arg_4]
               neg     edx
               neg     eax
               sbb     edx, 0
               retn    8
_RtlLargeIntegerNegate@8 endp


; ----------------------------------------------------------------------------
; Name      : RtlLargeIntegerSubtract
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.466
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlLargeIntegerSubtract@16        proc near

arg_0          = dword ptr  4
arg_4          = dword ptr  8
arg_8          = dword ptr  0Ch
arg_C          = dword ptr  10h

               mov     eax, [esp+arg_0]
               sub     eax, [esp+arg_8]
               mov     edx, [esp+arg_4]
               sbb     edx, [esp+arg_C]
               retn    10h
_RtlLargeIntegerSubtract@16        endp


; ----------------------------------------------------------------------------
; Name      : RtlConvertLongToLargeInteger
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.316
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlConvertLongToLargeInteger@4 proc near

arg_0          = dword ptr  4

               mov     eax, [esp+arg_0]
               cdq
               retn    4
_RtlConvertLongToLargeInteger@4 endp


; ----------------------------------------------------------------------------
; Name      : RtlConvertUlongToLargeInteger
; Purpose   :
; Parameters:
; Variables :
; Result    :
; Remark    : NTDLL.320
; Status    : VERIFIED
;
; Author    : Patrick Haller [Mon, 1999/11/08 23:44]
; ----------------------------------------------------------------------------
_RtlConvertUlongToLargeInteger@4 proc near

arg_0          = dword ptr  4

               mov     eax, [esp+arg_0]
               xor     edx, edx
               retn    4
_RtlConvertUlongToLargeInteger@4 endp



; ----------------------------------------------------------------------------

PUBLIC 		_POP_FPU
_POP_FPU proc near
	mov	eax, [esp+4]
	fstp	qword ptr [eax]
	ret
_POP_FPU endp

CODE32          ENDS

                END
