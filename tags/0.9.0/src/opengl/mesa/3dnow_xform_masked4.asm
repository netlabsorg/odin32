; $Id: 3dnow_xform_masked4.asm,v 1.1 2000-03-01 18:49:21 jeroen Exp $
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
ALIGN 16
PUBLIC _gl_3dnow_transform_points4_general_masked
_gl_3dnow_transform_points4_general_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 16]

    psllq mm7, 32
    por mm0, mm7

    movd mm1, [ECX + 4]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm1, mm7

    movd mm2, [ECX + 32]
    movd mm7, [ECX + 48]

    psllq mm7, 32
    por mm2, mm7

    movd mm3, [ECX + 36]
    movd mm7, [ECX + 52]

    psllq mm7, 32
    por mm3, mm7

    cmp ESI, 0
    je   G3TPGM_6

    push EBP
    push EAX
    push EDX
    push ESI

ALIGN 32

G3TPGM_2:
    test byte ptr [EBP], BL
    jnz   G3TPGM_3

    movq mm4, [EAX]
    movq mm5, mm4

    movq mm6, [EAX + 8]
    pfmul mm4, mm0

    movq mm7, mm6
    pfmul mm5, mm1

    pfmul mm6, mm2
    pfacc mm4, mm5

    pfmul mm7, mm3
    pfacc mm6, mm7

    pfadd mm6, mm4
    movq [EDX], mm6

G3TPGM_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TPGM_2

    movd mm0, [ECX + 8]
    movd mm7, [ECX + 24]

    psllq mm7, 32
    por mm0, mm7

    movd mm1, [ECX + 12]
    movd mm7, [ECX + 28]

    psllq mm7, 32
    por mm1, mm7

    movd mm2, [ECX + 40]
    movd mm7, [ECX + 56]

    psllq mm7, 32
    por mm2, mm7

    movd mm3, [ECX + 44]
    movd mm7, [ECX + 60]

    psllq mm7, 32
    por mm3, mm7

    pop ESI
    pop EDX
    pop EAX
    pop EBP

ALIGN 32

G3TPGM_4:
    test byte ptr [EBP], BL
    jnz   G3TPGM_5

    movq mm4, [EAX]
    movq mm5, mm4

    movq mm6, [EAX + 8]
    pfmul mm4, mm0

    movq mm7, mm6
    pfmul mm5, mm1

    pfmul mm6, mm2
    pfacc mm4, mm5

    pfmul mm7, mm3
    pfacc mm6, mm7

    pfadd mm6, mm4
    movq [EDX + 8], mm6

G3TPGM_5:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TPGM_4

G3TPGM_6:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_identity_masked
_gl_3dnow_transform_points4_identity_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

ALIGN 32

G3TPIM_2:
    test byte ptr [EBP], BL
    jnz   G3TPIM_3

    movq mm0, [EAX]
    movq [EDX], mm0

    movq mm1, [EAX + 8]
    movq [EDX + 8], mm1

G3TPIM_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TPIM_2

G3TPIM_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points4_2d_masked
_gl_3dnow_transform_points4_2d_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 16]

    psllq mm7, 32
    por mm0, mm7

    movd mm1, [ECX + 4]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm1, mm7

    movq mm2, [ECX + 48]
    cmp ESI, 0
    je   G3TP2M_4

ALIGN 32

G3TP2M_2:
    test byte ptr [EBP], BL
    jnz   G3TP2M_3

    movq mm3, [EAX]
    movq mm4, mm3

    movq mm5, [EAX + 8]
    pfmul mm3, mm0

    movq mm6, mm5
    pfmul mm4, mm1

    punpckhdq mm6, mm6
    pfacc mm3, mm4

    pfmul mm6, mm2
    pfadd mm3, mm6

    movq [EDX], mm3
    movq [EDX + 8], mm5

G3TP2M_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TP2M_2

G3TP2M_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points4_2d_no_rot_masked
_gl_3dnow_transform_points4_2d_no_rot_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm0, mm7

    movq mm1, [ECX + 48]
    cmp ESI, 0
    je   G3TP2NRM_4

ALIGN 32

G3TP2NRM_2:
    test byte ptr [EBP], BL
    jnz   G3TP2NRM_3

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    movq mm6, mm5
    pfmul mm4, mm0

    punpckhdq mm6, mm6
    pfmul mm6, mm1

    pfadd mm4, mm6
    movq [EDX], mm4

    movq [EDX + 8], mm5

G3TP2NRM_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TP2NRM_2

G3TP2NRM_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret


ALIGN 16
PUBLIC _gl_3dnow_transform_points4_3d_masked
_gl_3dnow_transform_points4_3d_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 16]

    psllq mm7, 32
    por mm0, mm7

    movd mm1, [ECX + 4]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm1, mm7

    movd mm2, [ECX + 32]
    movd mm7, [ECX + 48]

    psllq mm7, 32
    por mm2, mm7

    movd mm3, [ECX + 36]
    movd mm7, [ECX + 52]

    psllq mm7, 32
    por mm3, mm7

    cmp ESI, 0
    je   G3TP3M_6

    push EBP
    push EAX
    push EDX
    push ESI

ALIGN 32

G3TP3M_2:
    test byte ptr [EBP], BL
    jnz   G3TP3M_3

    movq mm4, [EAX]
    movq mm5, mm4

    pfmul mm4, mm0
    movq mm6, [EAX + 8]

    pfmul mm5, mm1
    movq mm7, mm6

    pfmul mm6, mm2
    pfacc mm4, mm5

    pfmul mm7, mm3
    pfacc mm6, mm7

    pfadd mm6, mm4
    movq [EDX], mm6

G3TP3M_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TP3M_2

    movd mm0, [ECX + 8]
    movd mm7, [ECX + 24]

    psllq mm7, 32
    por mm0, mm7

    movd mm2, [ECX + 40]
    movd mm7, [ECX + 56]

    psllq mm7, 32
    por mm2, mm7

    pop ESI
    pop EDX

    pop EAX
    pop EBP

ALIGN 32

G3TP3M_4:
    test byte ptr [EBP], BL
    jnz   G3TP3M_5

    movq mm4, [EAX]
    movq mm6, [EAX + 8]

    pfmul mm4, mm0
    movd mm7, [EAX + 12]

    pfmul mm6, mm2
    pfacc mm6, mm4

    pfacc mm6, mm7
    movq [EDX + 8], mm6

G3TP3M_5:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TP3M_4

G3TP3M_6:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_3d_no_rot_masked
_gl_3dnow_transform_points4_3d_no_rot_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm0, mm7

    movd mm2, [ECX + 40]
    movd mm3, [ECX + 56]

    psllq mm3, 32
    por mm2, mm3

    movq mm1, [ECX + 48]

    cmp ESI, 0
    je   G3TP3NRM_4

ALIGN 32

G3TP3NRM_2:
    test byte ptr [EBP], BL
    jnz   G3TP3NRM_3

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    movq mm6, mm5
    pfmul mm4, mm0

    movd mm7, [EAX + 12]
    pfmul mm5, mm2

    punpckhdq mm6, mm6
    pfacc mm5, mm7

    pfmul mm6, mm1
    pfadd mm4, mm6

    movq [EDX], mm4
    movq [EDX + 8], mm5

G3TP3NRM_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TP3NRM_2

G3TP3NRM_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points4_perspective_masked
_gl_3dnow_transform_points4_perspective_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push ESI
    push EDI
    push EBX
    push EBP

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]
    mov EBP, dword ptr [ESP + 36]
    mov BL, byte ptr [ESP + 40]

    femms

    movd mm0, [ECX]
    movd mm7, [ECX + 20]

    psllq mm7, 32
    por mm0, mm7

    movd mm1, [ECX + 40]
    movd mm7, [ECX + 56]

    psllq mm7, 32
    por mm1, mm7

    movq mm2, [ECX + 32]
    punpckldq mm7, mm7

    cmp ESI, 0
    je   G3TPPM_4

ALIGN 32

G3TPPM_2:
    test byte ptr [EBP], BL
    jnz   G3TPPM_3

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    pfmul mm4, mm0
    movq mm6, mm5

    punpckldq mm5, mm5
    pfmul mm6, mm1

    pfmul mm5, mm2
    pfadd mm5, mm4

    movq [EDX], mm5
    movd mm5, [EAX + 8]

    pfsubr mm5, mm7
    pfacc mm6, mm5

    movq [EDX + 8], mm6

G3TPPM_3:
    add EAX, EDI
    add EDX, 16

    inc EBP
    dec ESI

    ja   G3TPPM_2

G3TPPM_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret

CODE32 ENDS

end
