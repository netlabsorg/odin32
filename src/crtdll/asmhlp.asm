; $Id: asmhlp.asm,v 1.1 1999-12-21 12:27:11 sandervl Exp $

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

CODE32          ENDS

                END
