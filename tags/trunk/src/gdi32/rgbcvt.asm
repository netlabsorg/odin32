; $Id: rgbcvt.asm,v 1.5 2001-07-09 18:07:39 sandervl Exp $
; Copyright 2000 Daniela Engert (dani@ngrt.de)

	.586p
	.MMX
	.MODEL FLAT

	.DATA

	align	4
  and1mask dd	0001F001Fh
	   dd   0001F001Fh
  and2mask dd   0FFC0FFC0h
           dd   0FFC0FFC0h
  and2mask565 dd 0FFE07FE0h
              dd 07FE07FE0h

	.CODE

; void _Optlink RGB555to565 (WORD *dest, WORD *src, ULONG num);

	PUBLIC RGB555to565

RGB555to565 PROC NEAR

	push	esi
	push	edi
	cld

	mov	edi, eax		; _Optlink arg1 = EAX
	mov	esi, edx		; _Optlink arg2 = EDX
	shr	ecx, 1			; _Optlink arg3 = ECX
	pushf
cvt:
	mov	eax, [esi]
	mov	edx, eax

	add	esi, 4
	shl	eax, 1

	and	edx, 0001F001Fh
	and	eax, 0FFC0FFC0h

	add	edi, 4
	or	eax, edx

	mov	[edi-4], eax
	loop	cvt

	popf
	jnc	SHORT done

	mov	ax, [esi]
	mov	dx, ax
	and	dx, 0001Fh
	shl	ax, 1
	and	ax, 0FFC0h
	or	ax, dx
	mov	[edi], ax

done:
	pop	edi
	pop	esi
	ret

RGB555to565 ENDP

	PUBLIC RGB565to555

RGB565to555 PROC NEAR

	push	esi
	push	edi
	cld

	cmp	ecx, 0
	jz	done

	mov	edi, eax		; _Optlink arg1 = EAX
	mov	esi, edx		; _Optlink arg2 = EDX
	shr	ecx, 1			; _Optlink arg3 = ECX
	pushf
cvt:
	mov	eax, [esi]
	mov	edx, eax

	add	esi, 4
	shr	eax, 1

	and	edx, 0001F001Fh
	and	eax, 0FFE07FE0h

	add	edi, 4
	or	eax, edx

	mov	[edi-4], eax
	loop	cvt

	popf
	jnc	SHORT done

	mov	ax, [esi]
	mov	dx, ax
	and	dx, 0001Fh
	shr	ax, 1
	and	ax, 0FFE0h
	or	ax, dx
	mov	[edi], ax

done:
	pop	edi
	pop	esi
	ret

RGB565to555 ENDP

; void _Optlink RGB555to565MMX(WORD *dest, WORD *src, ULONG num);

	PUBLIC RGB555to565MMX

RGB555to565MMX PROC NEAR
	push	esi
	push	edi
	cld

	cmp	ecx, 0
	jz	done

	push	ecx
	
	mov	edi, eax		; _Optlink arg1 = EAX
	mov	esi, edx		; _Optlink arg2 = EDX
	shr	ecx, 3			; _Optlink arg3 = ECX
	jz	lastpixels

	sub	esp, 108
	fsaved	dword ptr [esp]

	movq	mm2, qword ptr and1mask		;  0001F001F001F001Fh
	movq	mm3, qword ptr and2mask		;  FFC0FFC00FFC0FFC0h

cvt:
	movq	mm0, qword ptr [esi]
	add	edi, 16

	movq	mm4, qword ptr [esi+8]
	movq	mm1, mm0

	movq	mm5, mm4
	psllq	mm0, 1

	psllq	mm4, 1
	pand	mm1, mm2

	pand	mm0, mm3
	pand	mm5, mm2

	pand	mm4, mm3
	por	mm0, mm1

	por	mm4, mm5
	add	esi, 16

	movq	qword ptr [edi-16], mm0
	dec	ecx

	movq	qword ptr [edi-8], mm4
	jnz	cvt

	nop
	nop

	frstord	dword ptr [esp]
	add	esp, 108

lastpixels:
	pop	ecx
	and	ecx, 3
	jz	short done

cvt2loop:
	mov	ax, [esi]
	mov	dx, ax

	add	esi, 2
	and	dx, 001Fh

	shl	ax, 1
	add	edi, 2

	and	ax, 0FFC0h
	or	ax, dx

	mov	[edi-2], ax
	loop	cvt2loop

done:
	pop	edi
	pop	esi
	ret

RGB555to565MMX ENDP


; void _Optlink RGB565to555MMX(WORD *dest, WORD *src, ULONG num);

	PUBLIC RGB565to555MMX

RGB565to555MMX PROC NEAR
	push	esi
	push	edi
	cld

	cmp	ecx, 0
	jz	done

	push	ecx
	
	mov	edi, eax		; _Optlink arg1 = EAX
	mov	esi, edx		; _Optlink arg2 = EDX
	shr	ecx, 3			; _Optlink arg3 = ECX
	jz	lastpixels

	sub	esp, 108
	fsaved	dword ptr [esp]

	movq	mm2, qword ptr and1mask			;  0001F001F001F001Fh
	movq	mm3, qword ptr and2mask565		;  FFE07FE007FE07FE0h

cvt:
	movq	mm0, qword ptr [esi]
	add	edi, 16

	movq	mm4, qword ptr [esi+8]
	movq	mm1, mm0

	movq	mm5, mm4
	psrlq	mm0, 1

	psrlq	mm4, 1
	pand	mm1, mm2

	pand	mm0, mm3
	pand	mm5, mm2

	pand	mm4, mm3
	por	mm0, mm1

	por	mm4, mm5
	add	esi, 16

	movq	qword ptr [edi-16], mm0
	dec	ecx

	movq	qword ptr [edi-8], mm4
	jnz	cvt

	nop
	nop

	frstord	dword ptr [esp]
	add	esp, 108

lastpixels:
	pop	ecx
	and	ecx, 3
	jz	short done

cvt2loop:
	mov	ax, [esi]
	mov	dx, ax

	add	esi, 2
	and	dx, 001Fh

	shr	ax, 1
	add	edi, 2

	and	ax, 0FFE0h
	or	ax, dx

	mov	[edi-2], ax
	loop	cvt2loop

done:
	pop	edi
	pop	esi
	ret

RGB565to555MMX ENDP

	END
