; $Id: asmhlp.asm,v 1.1 2000-11-21 23:48:54 phaller Exp $

.586P
               NAME    asmhlp

CODE32         SEGMENT DWORD PUBLIC USE32 'CODE'
               align 4

PUBLIC 		_POP_FPU
_POP_FPU proc near
	mov	eax, [esp+4]
	fstp	qword ptr [eax]
	ret
_POP_FPU endp

public		_SetEDX
_SetEDX proc near
	mov	edx, [esp+4];
	ret
_SetEDX endp

CODE32          ENDS

                END
