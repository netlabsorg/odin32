; $Id: cpuhlp.asm,v 1.4 2000-10-03 17:28:29 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * CPU detection functions
; *
; * Copyright 1999 Sander van Leeuwen
; *
; */
.586
                NAME    cpuhlp

DATA32  SEGMENT DWORD PUBLIC USE32 'DATA'
        ASSUME  DS:FLAT,SS:FLAT

	PUBLIC  _CPUFeatures
_CPUFeatures    dd 0

DATA32  ENDS

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

	public _SupportsCPUID

_SupportsCPUID proc near
       	pushfd
       	push    ecx

       	pushfd
       	pop     eax
       	mov     ecx, eax
       	xor     eax, 200000h    		;flip bit 21
       	push    eax             
       	popfd                   
       	pushfd                  
       	pop     eax             
       	cmp     eax, ecx			;bit 21 changed -> has cpuid support
       	jnz     @supportscpuid
       	xor	eax, eax
	jmp	short @end

@supportscpuid:
	mov	eax, 1
@end:
       	pop     ecx
       	popfd
	ret
_SupportsCPUID endp

	public _GetCPUVendorString

_GetCPUVendorString proc near
	push	ebp
	mov	ebp, esp
	push	ebx
	push	ecx
	push	edx
	push	esi

	mov  	esi, [ebp+8]	;ptr to string
	mov	eax, 0
	cpuid
	
	mov	[esi], ebx
	mov	[esi+4], edx
	mov	[esi+8], ecx

	pop	esi
	pop	edx
	pop	ecx
	pop	ebx
        pop  	ebp
	ret
_GetCPUVendorString endp

	public _GetCPUFeatures
_GetCPUFeatures proc near
	push	ebx
	push	ecx
	push	edx

	mov	eax, 1
	cpuid

	mov	eax, edx	

	mov	[_CPUFeatures], eax

	pop	edx
	pop	ecx
	pop	ebx
	ret
_GetCPUFeatures endp

	public _GetCPUSignature
_GetCPUSignature proc near
	push	ebx
	push	ecx
	push	edx

	mov	eax, 1
	cpuid

	pop	edx
	pop	ecx
	pop	ebx
	ret
_GetCPUSignature endp


_GetTSC proc near
	push	ebp
	mov	ebp, esp
	push	edx
	push	esi
	push	edi
	rdtsc

	mov  	esi, [ebp+8]	;ptr to low dword 
	mov	edi, [ebp+12]	;ptr to high dword
	mov	[esi], eax
	mov	[edi], edx

	pop	edi
	pop	esi
	pop	edx
	pop	ebp
	ret
_GetTSC endp

CODE32          ENDS

                END
