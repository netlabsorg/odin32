; $Id: cpuhlp.asm,v 1.1 1999-11-23 19:31:35 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * CPU detection functions
; *
; * Copyright 1999 Sander van Leeuwen
; *
; */
.586
                NAME    except

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

	public SupportsCPUID

SupportsCPUID proc near
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
SupportsCPUID endp

	public GetCPUVendorString

GetCPUVendorString proc near
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
GetCPUVendorString endp

	public GetCPUFeatures
GetCPUFeatures proc near
	push	ebx
	push	ecx
	push	edx

	mov	eax, 1
	cpuid

	mov	eax, edx	

	pop	edx
	pop	ecx
	pop	ebx
	ret
GetCPUFeatures endp

	public GetCPUSignature
GetCPUSignature proc near
	push	ebx
	push	ecx
	push	edx

	mov	eax, 1
	cpuid

	pop	edx
	pop	ecx
	pop	ebx
	ret
GetCPUSignature endp


CODE32          ENDS

                END
