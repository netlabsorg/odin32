; $Id: interlock.asm,v 1.3 1999-12-06 08:34:59 phaller Exp $

;/*
; * Interlocked apis
; *
; * IMPORTANT NOTE: THIS CODE WILL NOT RUN ON 386 CPUs,
; *                 486 IS MINIMUM REQUIREMENT!
; *
; * Copyright 1999 Sander van Leeuwen
; * Copyright 1999 Patrick Haller
; *
; * Project Odin Software License can be found in LICENSE.TXT
; *
; */
.486P
               NAME    interlocked

CODE32         SEGMENT DWORD PUBLIC USE32 'CODE'


;************************************************************************
;*           InterlockedIncrement			[KERNEL32]	*
;*									*
;* InterlockedIncrement adds 1 to a long variable and returns		*
;*  -  a negative number if the result < 0				*
;*  -  zero if the result == 0						*
;*  -  a positive number if the result > 0				*
;*									*
;* The returned number need not be equal to the result!!!!		*
;************************************************************************

       public  _InterlockedIncrement@4
_InterlockedIncrement@4 proc near

;@@@PH NT4 implementation
         mov      ecx, [esp+4]
         mov      eax, 1
         nop
    lock xadd     dword ptr [ecx], eax
         inc      eax
         retn 4

; @@@PH 1999/12/06 old implementation from W95
;var_4   = dword ptr -4
;arg_0   = dword ptr  4
;
;        sub     esp, 4
;        mov     eax, [esp + 4 + arg_0]
;   lock inc     dword ptr [eax]
;        mov     ecx, 0ffffffffh
;        js      _ic_2
;        jz      _ic_1
;        inc     ecx
;_ic_1:  inc     ecx
;_ic_2:  mov     [esp + 4 + var_4], ecx
;        mov     eax, ecx
;        retn    4
;
; @@@PH 1999/12/06 old implementation from WINE
;               mov	eax, dword ptr [esp+4] ; LPLONG lpAddend
;  	lock 	inc dword ptr [eax]
;       	mov 	eax, 0
;       	je   	@end                  ; not incremented?
;       	jl   	@decr
;       	inc 	eax                    ; ?
;       	jmp  	@end
;@decr: 	dec 	eax                    ; ?
;@end: 	ret 	4
_InterlockedIncrement@4 endp


;************************************************************************
;*           InterlockedDecrement			[KERNEL32]	*
;*									*
;* InterlockedIncrement adds 1 to a long variable and returns		*
;*  -  a negative number if the result < 0				*
;*  -  zero if the result == 0						*
;*  -  a positive number if the result > 0				*
;*									*
;* The returned number need not be equal to the result!!!!		*
;************************************************************************

       public  _InterlockedDecrement@4
_InterlockedDecrement@4 proc near

;@@@PH NT4 implementation
         mov      ecx, [esp+4]
         mov      eax, 0ffffffffh
         nop
    lock xadd     dword ptr [ecx], eax
         dec      eax
         retn 4


; @@@PH 1999/12/06 old implementation from W95
;var_4  = dword ptr -4
;arg_0  = dword ptr  4
;
;        sub     esp, 4
;        mov     eax, dword ptr[esp+4+arg_0]
;   lock dec     dword ptr [eax]
;        mov     ecx, 0ffffffffh
;        js      _id_2
;        jz      _id_1
;        inc     ecx
;_id_1:  inc     ecx
;_id_2:  mov     dword ptr [esp+4+var_4], ecx
;        mov     eax, ecx
;        add     esp, 4
;        retn    4

; @@@PH 1999/12/06 old implementation from WINE
;               mov	eax, dword ptr [esp+4] ; LPLONG lpAddend
;  	lock 	dec dword ptr [eax]
;       	mov 	eax, 0
;       	je   	@end                   ; not decremented?
;       	jl   	@decr
;       	inc 	eax                    ; ?
;       	jmp  	@end
;@decr:	dec 	eax                    ; ?
;@end: 	ret 	4

_InterlockedDecrement@4 endp


;************************************************************************
;*         InterlockedExchange				[KERNEL32.???]  *
;*                                                                      *
;* Atomically exchanges a pair of values.                               *
;*                                                                      *
;* RETURNS                                                              *
;*	Prior value of value pointed to by Target                       *
;************************************************************************

       public  _InterlockedExchange@8
_InterlockedExchange@8 proc near

;@@@PH NT4 implementation
         mov      ecx, [esp+8]
         mov      edx, [esp+4]
         mov      eax, dword ptr [ecx]
_ie_1:
         nop
    lock cmpxchg  dword ptr [ecx], edx
         jnz      _ie_1
         retn 8

; @@@PH 1999/12/06 old implementation from W95
;var_4   = dword ptr -4
;arg_0   = dword ptr  8
;arg_4   = dword ptr  0Ch
;
;        push    ebp
;        mov     ebp, esp
;        sub     esp, 4
;        mov     eax, dword ptr [ebp+arg_0]
;        mov     ecx, dword ptr [ebp+arg_4]
;        xchg    ecx, dword ptr [eax]
;        mov     dword ptr [ebp+var_4], ecx
;        mov     eax, ecx
;        mov     esp, ebp
;        pop     ebp
;        retn    8

; @@@PH 1999/12/06 old implementation from WINE
;       push	edx
;       mov	eax, [esp+12]        	; LONG value
;       mov     edx,[esp+8]           	; LPLONG target
; 	lock 	xchg eax, dword ptr [edx]
;       pop	edx
;      	ret 	8
_InterlockedExchange@8 endp



		public _InterlockedCompareExchange@12
;************************************************************************
;*           InterlockedCompareExchange		[KERNEL32.879]
;*
;* Atomically compares Destination and Comperand, and if found equal exchanges
;* the value of Destination with Exchange
;*
;* RETURNS
;*	Prior value of value pointed to by Destination
;*
;************************************************************************
_InterlockedCompareExchange@12 proc near

;@@@PH NT4 implementation
        mov     ecx, dword ptr [esp + 12]
        mov     edx, dword ptr [esp + 8]
        mov     eax, dword ptr [esp + 4]
_ice_1:
        nop
   lock cmpxchg dword ptr [ecx], edx
        retn 12


; @@@PH 1999/12/06 old implementation from WINE
;	push	ebp
;	mov	ebp, esp
;	push	edx
;	push	ebx
;
;	mov	ebx, dword ptr [ebp+8]  ;PVOID *Destination, /* Address of 32-bit value to exchange */
;	push	dword ptr [ebx]		;save old *Destination
;	mov	eax, [ebp+16]		;PVOID Comperand      /* value to compare, 32 bits */
;	mov	edx, [ebp+12]   	;PVOID Exchange,      /* change value, 32 bits */
;	lock	cmpxchg dword ptr [ebx],edx
;	pop	eax
;
;	pop	ebx
;	pop	edx
;	pop	ebp
;	ret	12
_InterlockedCompareExchange@12 endp


;************************************************************************
;*           InterlockedExchangeAdd			[KERNEL32.880]
;*
;* Atomically adds Increment to Addend and returns the previous value of
;* Addend
;*
;* RETURNS
;*	Prior value of value pointed to by cwAddendTarget
;*
;************************************************************************
		public _InterlockedExchangeAdd@8
_InterlockedExchangeAdd@8 proc near

;@@@PH NT4 implementation
        mov     ecx, dword ptr [esp + 4]
        mov     eax, dword ptr [esp + 8]
        nop
   lock xadd    dword ptr [ecx], eax
        retn    8

; @@@PH 1999/12/06 old implementation from WINE
;
;	push	edx
;	mov	eax, dword ptr [esp+12] ;LONG Increment /* Value to add */
;	mov	edx, dword ptr [esp+8]	;PLONG Addend, /* Address of 32-bit value to exchange */
;	lock	xadd dword ptr [edx], eax
;	pop	edx
;	ret 	8
_InterlockedExchangeAdd@8 endp

CODE32          ENDS

                END
