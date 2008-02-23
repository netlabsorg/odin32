; $Id: vertex_3dnow.asm,v 1.1 2000-03-01 18:49:39 jeroen Exp $
;
; 3dNow! transforms
;

        .586p

CODE32 SEGMENT PARA USE32 PUBLIC 'CODE'
CODE32 ENDS
DATA32 SEGMENT PARA USE32 PUBLIC 'DATA'
ASSUME DS:FLAT, SS:FLAT, ES:FLAT
DATA32 ENDS

CODE32 SEGMENT
PUBLIC _gl_3dnow_project_vertices
_gl_3dnow_project_vertices:

    push EBP
    femms
    prefetch [ESP + 8]

    mov ECX, dword ptr [ESP + 8]
    mov EDX, dword ptr [ESP + 12]
    mov EBP, dword ptr [ESP + 16]
    mov EAX, dword ptr [ESP + 20]

    movd mm6, [ EBP + 48]
    punpckldq mm6, [ EBP + 52]
    movd mm5, [EBP]
    punpckldq mm5, [ EBP + 20]
    movd mm1, [ EBP + 40]
    sub EDX, ECX

ALIGN 32
v16_3dnow_pv_loop_start:

    prefetch [ECX + 64]
    movd mm0, [ECX + 12]
    pfrcp mm0, mm0
    movd mm7, [ECX + 12]
    pfrcpit1 mm7, mm0
    pfrcpit2 mm7, mm0
    punpckldq mm7, mm7
    movq mm2, [ECX]
    pfmul mm2, mm7
    movd mm3, [ECX + 8]
    pfmul mm3, mm7
    movd mm0, [ EBP + 56]
    pfmul mm3, mm1
    pfadd mm3, mm0
    pfmul mm2, mm5
    pfadd mm2, mm6
    punpckldq mm3, mm7
    movq [ECX + 0], mm2
    movq [ECX + 8], mm3
    add ECX, EAX
    sub EDX, EAX
    ja   v16_3dnow_pv_loop_start

    femms
    pop EBP
    ret








PUBLIC _gl_3dnow_project_clipped_vertices
_gl_3dnow_project_clipped_vertices:

    push EBP
    push ESI

    femms

    prefetch [ESP + 12]

    mov ECX, dword ptr [ESP + 12]
    mov EDX, dword ptr [ESP + 16]
    mov EBP, dword ptr [ESP + 20]
    mov EAX, dword ptr [ESP + 24]
    mov ESI, dword ptr [ESP + 28]


    movd mm6, [ EBP + 48]
    punpckldq mm6, [ EBP + 52]
    movd mm5, [EBP]
    punpckldq mm5, [ EBP + 20]
    movd mm1, [ EBP + 40]


ALIGN 32
v16_3dnow_pcv_loop_start:

    cmp byte ptr [ESI], 0
    jne   v16_3dnow_pcv_skip

    movd mm0, [ECX + 12]
    pfrcp mm0, mm0
    movd mm7, [ECX + 12]
    pfrcpit1 mm7, mm0
    pfrcpit2 mm7, mm0
    punpckldq mm7, mm7
    movq mm2, [ECX]
    pfmul mm2, mm7
    movd mm3, [ECX + 8]
    pfmul mm3, mm7
    movd mm0, [ EBP + 56]
    pfmul mm3, mm1
    pfadd mm3, mm0
    pfmul mm2, mm5
    pfadd mm2, mm6
    punpckldq mm3, mm7
    movq [ECX + 0], mm2
    movq [ECX + 8], mm3

v16_3dnow_pcv_skip:
    add ECX, EAX
    inc ESI

    cmp EDX, ECX
    jne   v16_3dnow_pcv_loop_start

    femms

    pop ESI
    pop EBP
    ret






PUBLIC _gl_v16_3dnow_general_xform
_gl_v16_3dnow_general_xform:


        push EDI
        push ESI

        mov EAX, dword ptr [ESP + 12]
        mov ESI, dword ptr [ESP + 16]
        mov EDX, dword ptr [ESP + 20]
        mov EDI, dword ptr [ESP + 24]
        mov ECX, dword ptr [ESP + 28]

        femms

        movq mm7, [ ESI + 48]
        movq mm3, [ ESI + 56]

ALIGN 32
v16_3dnow_general_loop:
        prefetch [EAX + 128]
        prefetch [EDX + 32]

        movq mm0, [EDX]
        movd mm1, [EDX + 8]
        movq mm4, [ESI]
        punpckhdq mm2, mm0
        movq mm5, [ESI + 16]
        punpckldq mm0, mm0
        movq mm6, [ESI + 32]
        pfmul mm4, mm0
        punpckhdq mm2, mm2
        pfmul mm5, mm2
        punpckldq mm1, mm1
        pfmul mm0, [ESI + 8]
        pfmul mm2, [ESI + 24]
        pfmul mm6, mm1
        pfadd mm5, mm4
        pfmul mm1, [ESI + 40]
        pfadd mm2, mm0
        pfadd mm6, mm5
        pfadd mm2, mm1
        pfadd mm6, mm7
        pfadd mm2, mm3
        add EDX, EDI
        movq [EAX], mm6
        movq [EAX + 8], mm2
        add EAX, 64
        dec ECX
        jne   v16_3dnow_general_loop

        femms

        pop ESI
        pop EDI
        ret

CODE32 ENDS

end
