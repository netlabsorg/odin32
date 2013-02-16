	extrn	_dllentry:dword
	title	large_int.c
	.386
	.387
CODE32	segment para use32 public 'CODE'
CODE32	ends
DATA32	segment para use32 public 'DATA'
DATA32	ends
CONST32_RO	segment para use32 public 'CONST'
CONST32_RO	ends
BSS32	segment para use32 public 'BSS'
BSS32	ends
DGROUP	group BSS32, DATA32
	assume	cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT
CODE32	segment
	extrn	__modu64:proc

; 224 ULONGLONG WINAPI _aullrem( ULONGLONG a, ULONGLONG b )
	align 4

	public __aullrem@16
__aullrem@16	proc

; 226     return a % b;
	mov	ecx,[esp+010h];	b
	push	ecx
	mov	eax,[esp+08h];	a
	mov	edx,[esp+0ch];	a
	mov	ecx,[esp+010h];	b
	call	__modu64
	pop	ecx
	ret	010h
__aullrem@16	endp
	extrn	__divu64:proc

; 215 ULONGLONG WINAPI _aulldiv( ULONGLONG a, ULONGLONG b )
	align 4h

	public __aulldiv@16
__aulldiv@16	proc

; 217     return a / b;
	mov	ecx,[esp+010h];	b
	push	ecx
	mov	ecx,[esp+010h];	b
	mov	edx,[esp+0ch];	a
	mov	eax,[esp+08h];	a
	call	__divu64
	pop	ecx
	ret	010h
__aulldiv@16	endp
	extrn	__modi64:proc

; 206 LONGLONG WINAPI _allrem( LONGLONG a, LONGLONG b )
	align 4h

	public __allrem@16
__allrem@16	proc

; 208     return a % b;
	mov	ecx,[esp+010h];	b
	push	ecx
	mov	eax,[esp+08h];	a
	mov	edx,[esp+0ch];	a
	mov	ecx,[esp+010h];	b
	call	__modi64
	pop	ecx
	ret	010h
__allrem@16	endp
	extrn	__multi64:proc

; 197 LONGLONG WINAPI _allmul( LONGLONG a, LONGLONG b )
	align 4h

	public __allmul@16
__allmul@16	proc

; 199     return a * b;
	mov	ecx,[esp+010h];	b
	push	ecx
	mov	ecx,[esp+010h];	b
	mov	edx,[esp+0ch];	a
	mov	eax,[esp+08h];	a
	call	__multi64
	pop	ecx
	ret	010h
__allmul@16	endp
	extrn	__divi64:proc

; 188 LONGLONG WINAPI _alldiv( LONGLONG a, LONGLONG b )
	align 4h

	public __alldiv@16
__alldiv@16	proc

; 190     return a / b;
	mov	ecx,[esp+010h];	b
	push	ecx
	sub	esp,0ch
	mov	ecx,[esp+01ch];	b
	mov	edx,[esp+018h];	a
	mov	eax,[esp+014h];	a
	call	__divi64
	add	esp,010h
	ret	010h
__alldiv@16	endp

; 158 LONGLONG WINAPI RtlExtendedIntegerMultiply( LONGLONG a, INT b )
	align 4h

	public _RtlExtendedIntegerMultiply@12
_RtlExtendedIntegerMultiply@12	proc

; 160     return a * b;
	mov	ecx,[esp+08h];	a
	push	ecx
	mov	eax,[esp+010h];	b
	mov	ecx,[esp+08h];	a
	mov	edx,eax
	sar	edx,01fh
	call	__multi64
	pop	ecx
	ret	0ch
_RtlExtendedIntegerMultiply@12	endp

; 147 LONGLONG WINAPI RtlExtendedLargeIntegerDivide( LONGLONG a, INT b, INT *rem )
	align 4h

	public _RtlExtendedLargeIntegerDivide@16
_RtlExtendedLargeIntegerDivide@16	proc
	sub	esp,08h
	push	ebx
	push	edi

; 149     LONGLONG ret = a / b;
	mov	ebx,[esp+01ch];	b
	mov	ecx,ebx
	sar	ecx,01fh
	push	ecx
	mov	ecx,ebx
	sub	esp,0ch
	mov	edi,[esp+024h];	a
	mov	edx,[esp+028h];	a
	mov	eax,edi
	call	__divi64
	add	esp,010h

; 150     if (rem) *rem = a - b * ret;
	mov	ecx,[esp+020h];	rem

; 149     LONGLONG ret = a / b;
	mov	[esp+08h],eax;	ret
	mov	[esp+0ch],edx;	ret

; 150     if (rem) *rem = a - b * ret;
	test	ecx,ecx
	je	@BLBL15
	imul	ebx,eax
	sub	edi,ebx
	mov	[ecx],edi
@BLBL15:

; 151     return ret;
	pop	edi
	pop	ebx
	add	esp,08h
	ret	010h
_RtlExtendedLargeIntegerDivide@16	endp

; 127 UINT WINAPI RtlEnlargedUnsignedDivide( ULONGLONG a, UINT b, UINT *remptr )
	align 4h

	public _RtlEnlargedUnsignedDivide@16
_RtlEnlargedUnsignedDivide@16	proc
	push	ebp
	push	ebx

; 137     UINT ret = a / b;
	xor	ebx,ebx

; 127 UINT WINAPI RtlEnlargedUnsignedDivide( ULONGLONG a, UINT b, UINT *remptr )
	push	edi
	push	esi
	push	ebp

; 137     UINT ret = a / b;
	mov	esi,[esp+020h];	b
	push	ebx
	mov	ebp,[esp+020h];	a
	mov	edi,[esp+01ch];	a
	mov	ecx,esi
	mov	edx,ebp
	mov	eax,edi
	call	__divu64
	xchg	edi,eax
	mov	ecx,esi
	mov	edx,ebp
	pop	esi
	mov	[esp],edi;	ret

; 138     if (remptr) *remptr = a % b;
	mov	edi,[esp+024h];	remptr
	test	edi,edi
	je	@BLBL13
	push	ebx
	call	__modu64
	mov	[edi],eax
	pop	ecx
@BLBL13:

; 139     return ret;
	mov	eax,[esp];	ret
	pop	ecx
	pop	esi
	pop	edi
	pop	ebx
	pop	ebp
	ret	010h
_RtlEnlargedUnsignedDivide@16	endp

; 118 ULONGLONG WINAPI RtlEnlargedUnsignedMultiply( UINT a, UINT b )
	align 4h

	public _RtlEnlargedUnsignedMultiply@8
_RtlEnlargedUnsignedMultiply@8	proc

; 120     return (ULONGLONG)a * b;
	mov	ecx,[esp+04h];	a
	xor	eax,eax
	push	eax
	xor	edx,edx
	mov	eax,[esp+0ch];	b
	call	__multi64
	pop	ecx
	ret	08h
_RtlEnlargedUnsignedMultiply@8	endp

; 109 LONGLONG WINAPI RtlEnlargedIntegerMultiply( INT a, INT b )
	align 4h

	public _RtlEnlargedIntegerMultiply@8
_RtlEnlargedIntegerMultiply@8	proc

; 111     return (LONGLONG)a * b;
	mov	ecx,[esp+04h];	a
	mov	eax,ecx
	sar	eax,01fh
	push	eax
	mov	eax,[esp+0ch];	b
	mov	edx,eax
	sar	edx,01fh
	call	__multi64
	pop	ecx
	ret	08h
_RtlEnlargedIntegerMultiply@8	endp

; 100 ULONGLONG WINAPI RtlConvertUlongToLargeInteger( ULONG a )
	align 4h

	public _RtlConvertUlongToLargeInteger@4
_RtlConvertUlongToLargeInteger@4	proc

; 102     return a;
	mov	eax,[esp+04h];	a
	xor	edx,edx
	ret	04h
_RtlConvertUlongToLargeInteger@4	endp

; 91 LONGLONG WINAPI RtlConvertLongToLargeInteger( LONG a )
	align 4h

	public _RtlConvertLongToLargeInteger@4
_RtlConvertLongToLargeInteger@4	proc

; 93     return a;
	mov	eax,[esp+04h];	a
	mov	edx,eax
	sar	edx,01fh
	ret	04h
_RtlConvertLongToLargeInteger@4	endp

; 80 ULONGLONG WINAPI RtlLargeIntegerDivide( ULONGLONG a, ULONGLONG b, ULONGLONG *rem )
	align 4h

	public _RtlLargeIntegerDivide@20
_RtlLargeIntegerDivide@20	proc
	push	ebp
	push	ebx
	push	edi
	push	esi
	sub	esp,08h

; 82     ULONGLONG ret = a / b;
	mov	ebp,[esp+028h];	b
	push	ebp
	mov	ebx,[esp+028h];	b
	mov	esi,[esp+024h];	a
	mov	edi,[esp+020h];	a
	mov	ecx,ebx
	mov	edx,esi
	mov	eax,edi
	call	__divu64
	mov	ecx,ebx
	pop	ebx

; 83     if (rem) *rem = a - ret * b;
	mov	ebx,[esp+02ch];	rem

; 82     ULONGLONG ret = a / b;
	mov	[esp],eax;	ret
	mov	[esp+04h],edx;	ret

; 83     if (rem) *rem = a - ret * b;
	test	ebx,ebx
	je	@BLBL7
	push	ebp
	call	__multi64
	mov	ecx,edi
	pop	edi
	sub	ecx,eax
	sbb	esi,edx
	mov	[ebx],ecx
	mov	[ebx+04h],esi
@BLBL7:

; 84     return ret;
	mov	eax,[esp];	ret
	mov	edx,[esp+04h];	ret
	add	esp,08h
	pop	esi
	pop	edi
	pop	ebx
	pop	ebp
	ret	014h
_RtlLargeIntegerDivide@20	endp

; 68 LONGLONG WINAPI RtlLargeIntegerArithmeticShift( LONGLONG a, INT count )
	align 4h

	public _RtlLargeIntegerArithmeticShift@12
_RtlLargeIntegerArithmeticShift@12	proc

; 71     return a >> count;
	mov	ecx,[esp+0ch];	count
	mov	edx,[esp+08h];	a
	and	ecx,03fh
	mov	eax,[esp+04h];	a
	cmp	ecx,01fh
	jg	@BLBL23
	shrd	eax,edx,cl
	sar	edx,cl
	jmp	@BLBL24
@BLBL23:
	mov	eax,edx
	sub	cl,020h
	sar	edx,01fh
	sar	eax,cl
@BLBL24:
	ret	0ch
_RtlLargeIntegerArithmeticShift@12	endp

; 59 LONGLONG WINAPI RtlLargeIntegerShiftRight( LONGLONG a, INT count )
	align 4h

	public _RtlLargeIntegerShiftRight@12
_RtlLargeIntegerShiftRight@12	proc

; 61     return (ULONGLONG)a >> count;
	mov	ecx,[esp+0ch];	count
	mov	eax,[esp+04h];	a
	and	ecx,03fh
	mov	edx,[esp+08h];	a
	cmp	ecx,01fh
	jg	@BLBL25
	shrd	eax,edx,cl
	shr	edx,cl
	jmp	@BLBL26
@BLBL25:
	mov	eax,edx
	sub	cl,020h
	shr	eax,cl
	xor	edx,edx
@BLBL26:
	ret	0ch
_RtlLargeIntegerShiftRight@12	endp

; 50 LONGLONG WINAPI RtlLargeIntegerShiftLeft( LONGLONG a, INT count )
	align 4h

	public _RtlLargeIntegerShiftLeft@12
_RtlLargeIntegerShiftLeft@12	proc

; 52     return a << count;
	mov	ecx,[esp+0ch];	count
	mov	eax,[esp+04h];	a
	and	ecx,03fh
	mov	edx,[esp+08h];	a
	cmp	ecx,01fh
	jg	@BLBL27
	shld	edx,eax,cl
	shl	eax,cl
	jmp	@BLBL28
@BLBL27:
	mov	edx,eax
	sub	cl,020h
	shl	edx,cl
	xor	eax,eax
@BLBL28:
	ret	0ch
_RtlLargeIntegerShiftLeft@12	endp

; 41 LONGLONG WINAPI RtlLargeIntegerNegate( LONGLONG a )
	align 4h

	public _RtlLargeIntegerNegate@8
_RtlLargeIntegerNegate@8	proc

; 43     return -a;
	mov	eax,[esp+04h];	a
	mov	edx,[esp+08h];	a
	not	eax
	not	edx
	add	eax,01h
	adc	edx,0h
	ret	08h
_RtlLargeIntegerNegate@8	endp

; 32 LONGLONG WINAPI RtlLargeIntegerSubtract( LONGLONG a, LONGLONG b )
	align 4h

	public _RtlLargeIntegerSubtract@16
_RtlLargeIntegerSubtract@16	proc

; 34     return a - b;
	mov	eax,[esp+04h];	a
	mov	edx,[esp+08h];	a
	sub	eax,[esp+0ch];	b
	sbb	edx,[esp+010h];	b
	ret	010h
_RtlLargeIntegerSubtract@16	endp

; 23 LONGLONG WINAPI RtlLargeIntegerAdd( LONGLONG a, LONGLONG b )
	align 4h

	public _RtlLargeIntegerAdd@16
_RtlLargeIntegerAdd@16	proc

; 25     return a + b;
	mov	eax,[esp+04h];	a
	mov	edx,[esp+08h];	a
	add	eax,[esp+0ch];	b
	adc	edx,[esp+010h];	b
	ret	010h
_RtlLargeIntegerAdd@16	endp
CODE32	ends
end
