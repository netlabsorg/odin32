; $Id: vertex.asm,v 1.1 2000-03-01 18:49:39 jeroen Exp $
;
; Assembly versions of the general vertex transform
;

            .586p

CODE32 SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32 ENDS
DATA32 SEGMENT DWORD USE32 PUBLIC 'DATA'
ASSUME DS:FLAT, SS:FLAT, ES:FLAT
DATA32 ENDS

CODE32 SEGMENT

PUBLIC _gl_v16_x86_general_xform
_gl_v16_x86_general_xform:


        push EDI
        push ESI

        mov EAX, dword ptr [ESP + 12]
        mov ESI, dword ptr [ESP + 16]
        mov EDX, dword ptr [ESP + 20]
        mov EDI, dword ptr [ESP + 24]
        mov ECX, dword ptr [ESP + 28]


v16x86_loop:
        fld dword ptr [EDX + 0h]
        fmul dword ptr [ESI + 0h]
        fld dword ptr [EDX + 0h]
        fmul dword ptr [ESI + 4h]
        fld dword ptr [EDX + 0h]
        fmul dword ptr [ESI + 8h]
        fld dword ptr [EDX + 0h]
        fmul dword ptr [ESI + 0ch]
        fld dword ptr [EDX + 04h]
        fmul dword ptr [ESI + 010h]
        fld dword ptr [EDX + 04h]
        fmul dword ptr [ESI + 014h]
        fld dword ptr [EDX + 04h]
        fmul dword ptr [ESI + 018h]
        fld dword ptr [EDX + 04h]
        fmul dword ptr [ESI + 1ch]
        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)
        fld dword ptr [EDX + 08h]
        fmul dword ptr [ESI + 20h]
        fld dword ptr [EDX + 8h]
        fmul dword ptr [ESI + 24h]
        fld dword ptr [EDX + 8h]
        fmul dword ptr [ESI + 28h]
        fld dword ptr [EDX + 8h]
        fmul dword ptr [ESI + 2ch]
        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)
        fxch ST(3)
        fadd dword ptr [ESI + 30h]
        fxch ST(2)
        fadd dword ptr [ESI + 34h]
        fxch ST(1)
        fadd dword ptr [ESI + 38h]
        fxch ST(3)
        fadd dword ptr [ESI + 3ch]
        fxch ST(2)
        fstp dword ptr [EAX + 00h]
        fstp dword ptr [EAX + 04h]
        fxch ST(1)
        fstp dword ptr [EAX + 08h]
        fstp dword ptr [EAX + 0ch]
        add EAX, 64
        add EDX, EDI
        dec ECX
        jne   v16x86_loop

        pop ESI
        pop EDI
        ret

CODE32 ENDS

end
