; $Id: math64.asm,v 1.1 2001-07-20 15:41:43 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */

; 1999/08/09 PH see if we can do this as INLINE functions

.386p
                NAME    odinfs

DATA32	segment para use32 public 'DATA'
flt_A0CB4	dd 9.223372e18		; DATA XREF: ODINCRTD_1030+1Ar
DATA32	ends

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  DS:FLAT, SS:FLAT

		public __modi64
__modi64	proc near

arg_0		= dword	ptr  0Ch

		push	edi
		push	ebx
		mov	edi, [esp+arg_0]
		mov	ebx, 80000000h
		test	edx, ebx
		jz	short loc_25FEB
		not	eax
		not	edx
		add	eax, 1
		adc	edx, 0
		test	edi, ebx
		jz	short loc_26005
		not	ecx
		not	edi
		add	ecx, 1
		adc	edi, 0
		jmp	short loc_2600F
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_25FE1:				; CODE XREF: ODINCRTD_1027+35j
		mov	ebx, edi
		and	ebx, 80000000h
		jmp	short loc_2600F
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_25FEB:				; CODE XREF: ODINCRTD_1027+Dj
		test	edi, ebx
		jz	short loc_25FE1
		mov	ebx, edi
		not	ebx
		and	ebx, 80000000h
		not	ecx
		not	edi
		add	ecx, 1
		adc	edi, 0
		jmp	short loc_2600F
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26005:				; CODE XREF: ODINCRTD_1027+1Bj
		mov	ebx, edi
		not	ebx
		and	ebx, 80000000h

loc_2600F:				; CODE XREF: ODINCRTD_1027+27j
					; ODINCRTD_1027+31j ...
		test	edi, edi
		jnz	short loc_26047
		test	ecx, ecx
		jz	short loc_2601B
		cmp	ecx, edx
		jbe	short loc_26053

loc_2601B:				; CODE XREF: ODINCRTD_1027+5Dj
		div	ecx
		mov	eax, edx
		xor	edx, edx
		test	ebx, ebx
		jz	short loc_26087
		not	eax
		not	edx
		add	eax, 1
		adc	edx, 0
		jmp	short loc_26087
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26031:				; CODE XREF: ODINCRTD_1027+99j
		xor	edx, edx
		xor	eax, eax
		jmp	short loc_26087
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26037:				; CODE XREF: ODINCRTD_1027+93j
					; ODINCRTD_1027+97j
		test	ebx, ebx
		jz	short loc_26087
		not	eax
		not	edx
		add	eax, 1
		adc	edx, 0
		jmp	short loc_26087
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26047:				; CODE XREF: ODINCRTD_1027+59j
		cmp	edx, edi
		ja	short loc_26053
		jb	short loc_26037
		cmp	eax, ecx
		jb	short loc_26037
		jz	short loc_26031

loc_26053:				; CODE XREF: ODINCRTD_1027+61j
					; ODINCRTD_1027+91j
		push	ebp
		push	esi
		xor	esi, esi
		xor	ebp, ebp
		mov	bl, 40h	; '@'

loc_2605B:				; CODE XREF: ODINCRTD_1027+BBj
		shl	eax, 1
		rcl	edx, 1
		rcl	esi, 1
		rcl	ebp, 1
		cmp	ebp, edi
		ja	short loc_2606D
		jb	short loc_26071
		cmp	esi, ecx
		jb	short loc_26071

loc_2606D:				; CODE XREF: ODINCRTD_1027+ADj
		sub	esi, ecx
		sbb	ebp, edi

loc_26071:				; CODE XREF: ODINCRTD_1027+AFj
					; ODINCRTD_1027+B3j
		dec	bl
		jnz	short loc_2605B
		mov	eax, esi
		mov	edx, ebp
		test	ebx, ebx
		jz	short loc_26085
		xor	eax, eax
		xor	edx, edx
		sub	eax, esi
		sbb	edx, ebp

loc_26085:				; CODE XREF: ODINCRTD_1027+C3j
		pop	esi
		pop	ebp

loc_26087:				; CODE XREF: ODINCRTD_1027+6Bj
					; ODINCRTD_1027+77j ...
		pop	ebx
		pop	edi
		retn	
__modi64	endp

		public __modu64
__modu64	proc near		; CODE XREF: sub_473C0+46p
					; sub_4A490+348p ...

arg_0		= dword	ptr  8

		push	edi
		mov	edi, [esp+arg_0]
		test	edi, edi
		jnz	short loc_26B3F
		test	ecx, ecx
		jz	short loc_26B31
		cmp	ecx, edx
		jbe	short loc_26B4B

loc_26B31:				; CODE XREF: ODINCRTD_1028+Bj
		div	ecx
		mov	eax, edx
		xor	edx, edx
		jmp	short loc_26B78
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26B39:				; CODE XREF: ODINCRTD_1028+29j
		xor	eax, eax
		xor	edx, edx
		jmp	short loc_26B78
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26B3F:				; CODE XREF: ODINCRTD_1028+7j
		cmp	edx, edi
		ja	short loc_26B4B
		jb	short loc_26B78
		cmp	eax, ecx
		jb	short loc_26B78
		jz	short loc_26B39

loc_26B4B:				; CODE XREF: ODINCRTD_1028+Fj
					; ODINCRTD_1028+21j
		push	ebp
		push	ebx
		push	esi
		xor	esi, esi
		xor	ebp, ebp
		mov	bl, 40h	; '@'

loc_26B54:				; CODE XREF: ODINCRTD_1028+4Fj
		shl	eax, 1
		rcl	edx, 1
		rcl	esi, 1
		rcl	ebp, 1
		cmp	ebp, edi
		ja	short loc_26B66
		jb	short loc_26B6D
		cmp	esi, ecx
		jb	short loc_26B6D

loc_26B66:				; CODE XREF: ODINCRTD_1028+3Ej
		sub	esi, ecx
		sbb	ebp, edi
		or	eax, 1

loc_26B6D:				; CODE XREF: ODINCRTD_1028+40j
					; ODINCRTD_1028+44j
		dec	bl
		jnz	short loc_26B54
		mov	eax, esi
		mov	edx, ebp
		pop	esi
		pop	ebx
		pop	ebp

loc_26B78:				; CODE XREF: ODINCRTD_1028+17j
					; ODINCRTD_1028+1Dj ...
		pop	edi
		retn	
__modu64	endp

		public __multi64
__multi64	proc near		; CODE XREF: sub_259BA+258p
					; sub_259BA+337p ...

arg_0		= dword	ptr  8

		push	edx
		or	edx, [esp+arg_0]
		jnz	short loc_2D5E3
		mul	ecx
		pop	ecx
		retn	
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_2D5E3:				; CODE XREF: ODINCRTD_1029+5j
		push	ebx
		push	eax
		mul	[esp+8+arg_0]
		mov	ebx, eax
		mov	eax, ecx
		mul	dword ptr [esp+8]
		add	ebx, eax
		pop	eax
		mul	ecx
		add	edx, ebx
		pop	ebx
		pop	ecx
		retn	
__multi64	endp

		public __u64toflt
__u64toflt	proc near

var_C		= tbyte	ptr -0Ch

		mov	ecx, edx
		or	edx, 80000000h
		push	403Eh
		push	edx
		push	eax
		fld	[esp+0Ch+var_C]
		test	ecx, 80000000h
		jnz	short loc_22AD8
		fsub	flt_A0CB4

loc_22AD8:				; CODE XREF: ODINCRTD_1030+18j
		add	esp, 0Ch
		retn	
__u64toflt	endp

		public __divi64
__divi64	proc near		; CODE XREF: cseg02:0002E670p
					; cseg02:0002E6FAp

var_4		= word ptr -4
var_2		= word ptr -2
arg_0		= qword	ptr  4
arg_8		= qword	ptr  0Ch

		mov	dword ptr [esp+arg_0+4], edx
		or	edx, dword ptr [esp+arg_8+4]
		jnz	short loc_260D1
		xor	edx, edx
		div	ecx
		xor	edx, edx
		retn	
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_260D1:				; CODE XREF: ODINCRTD_1031+8j
		sub	esp, 4
		mov	dword ptr [esp+4+arg_0], eax
		mov	dword ptr [esp+4+arg_8], ecx
		fstcw	[esp+4+var_4]
		mov	ax, [esp+4+var_4]
		or	ax, 0F00h
		mov	[esp+4+var_2], ax
		wait	
		fldcw	[esp+4+var_2]
		fild	[esp+4+arg_0]
		fild	[esp+4+arg_8]
		fdivp	st(1), st
		fistp	[esp+4+arg_0]
		fclex	
		wait	
		fldcw	[esp+4+var_4]
		add	esp, 4
		mov	eax, dword ptr [esp+arg_0]
		mov	edx, dword ptr [esp+arg_0+4]
		retn	
__divi64	endp

		public __divu64
__divu64	proc near		; CODE XREF: cseg02:0002E2D6p
					; sub_473C0+2Ep	...

arg_0		= dword	ptr  8

		push	edi
		mov	edi, [esp+arg_0]
		test	edi, edi
		jnz	short loc_26D12
		test	ecx, ecx
		jz	short loc_26CFD
		cmp	ecx, edx
		jbe	short loc_26D1E

loc_26CFD:				; CODE XREF: ODINCRTD_1032+Bj
		div	ecx
		xor	edx, edx
		jmp	short loc_26D47
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26D03:				; CODE XREF: ODINCRTD_1032+2Aj
					; ODINCRTD_1032+2Ej
		xor	edx, edx
		xor	eax, eax
		jmp	short loc_26D47
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26D09:				; CODE XREF: ODINCRTD_1032+30j
		xor	edx, edx
		mov	eax, 1
		jmp	short loc_26D47
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

loc_26D12:				; CODE XREF: ODINCRTD_1032+7j
		cmp	edx, edi
		ja	short loc_26D1E
		jb	short loc_26D03
		cmp	eax, ecx
		jb	short loc_26D03
		jz	short loc_26D09

loc_26D1E:				; CODE XREF: ODINCRTD_1032+Fj
					; ODINCRTD_1032+28j
		push	ebp
		push	ebx
		push	esi
		xor	esi, esi
		xor	ebp, ebp
		mov	bl, 40h	; '@'

loc_26D27:				; CODE XREF: ODINCRTD_1032+56j
		shl	eax, 1
		rcl	edx, 1
		rcl	esi, 1
		rcl	ebp, 1
		cmp	ebp, edi
		ja	short loc_26D39
		jb	short loc_26D40
		cmp	esi, ecx
		jb	short loc_26D40

loc_26D39:				; CODE XREF: ODINCRTD_1032+45j
		sub	esi, ecx
		sbb	ebp, edi
		or	eax, 1

loc_26D40:				; CODE XREF: ODINCRTD_1032+47j
					; ODINCRTD_1032+4Bj
		dec	bl
		jnz	short loc_26D27
		pop	esi
		pop	ebx
		pop	ebp

loc_26D47:				; CODE XREF: ODINCRTD_1032+15j
					; ODINCRTD_1032+1Bj ...
		pop	edi
		retn	
__divu64	endp

CODE32          ENDS
                END
