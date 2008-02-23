	title	large_int_odin.c
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


; 23 LARGE_INTEGER WINAPI RtlpLargeIntegerAdd( LARGE_INTEGER *a, LARGE_INTEGER *b )
	align 4h

	public _RtlpLargeIntegerAdd@8
_RtlpLargeIntegerAdd@8	proc

; 25     return a + b;
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        push    ecx

	mov	edi,[ebp+08h];	result
        mov     esi,[ebp+0Ch];  a
        mov     ecx,[ebp+10h];  b

	mov	eax,[esi];	a
	mov	edx,[esi+4];	a
	add	eax,[ecx];	b
	adc	edx,[ecx+4];	b

        mov     [edi], eax
        mov     [edi+4], edx

        pop     ecx
        pop     edi
        pop     esi
        pop     ebp
	ret	0Ch
_RtlpLargeIntegerAdd@8	endp

CODE32	ends
end
