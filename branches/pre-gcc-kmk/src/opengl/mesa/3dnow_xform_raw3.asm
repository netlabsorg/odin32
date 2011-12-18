; $Id: 3dnow_xform_raw3.asm,v 1.1 2000-03-01 18:49:22 jeroen Exp $
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
PUBLIC _gl_3dnow_transform_points3_general_raw
_gl_3dnow_transform_points3_general_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TPGR_4

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movq mm2, [ECX + 32]
    movq mm3, [ECX + 48]

    push EAX
    push EDX
    push ESI

    movq mm4, [EAX]
    movd mm6, [EAX + 8]


ALIGN 32
G3TPGR_2:

    movq mm5, mm4
    add EAX, EDI

    pfmul mm4, mm0
    punpckldq mm6, mm6

    pfmul mm5, mm1
    pfacc mm4, mm5

    pfmul mm6, mm2
    pfadd mm6, mm3

    pfadd mm6, mm4
    add EDX, 16

    dec ESI
    movq [EDX + -16], mm6

    movq mm4, [EAX]
    movd mm6, [EAX + 8]

    ja   G3TPGR_2

    pop ESI
    pop EDX
    pop EAX

    movd mm0, [ECX + 8]
    punpckldq mm0, [ECX + 24]

    movd mm1, [ECX + 12]
    punpckldq mm1, [ECX + 28]

    movq mm2, [ECX + 40]
    movq mm3, [ECX + 56]

    movq mm4, [EAX]
    movd mm6, [EAX + 8]


ALIGN 32
G3TPGR_3:

    add EAX, EDI
    movq mm5, mm4

    pfmul mm4, mm0
    punpckldq mm6, mm6

    pfmul mm5, mm1
    pfacc mm4, mm5

    pfmul mm6, mm2
    pfadd mm6, mm3

    pfadd mm6, mm4
    add EDX, 16

    dec ESI
    movq [EDX + -8], mm6

    movq mm4, [EAX]
    movd mm6, [EAX + 8]

    ja   G3TPGR_3

G3TPGR_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret






ALIGN 16
PUBLIC _gl_3dnow_transform_points3_identity_raw
_gl_3dnow_transform_points3_identity_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TPIR_4

    movq mm0, [EAX]
    movd mm1, [EAX + 8]


ALIGN 32
G3TPIR_3:

    add EAX, EDI
    add EDX, 16

    dec ESI
    movq [EDX + -16], mm0

    movd [EDX + -8], mm1
    movq mm0, [EAX]

    movd mm1, [EAX + 8]
    ja   G3TPIR_3

G3TPIR_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points3_2d_raw
_gl_3dnow_transform_points3_2d_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TP2R_3

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movq mm2, [ECX + 48]
    movq mm3, [EAX]

    movd mm5, [EAX + 8]

ALIGN 32
G3TP2R_2:

    movq mm4, mm3
    pfmul mm3, mm0

    add EAX, EDI
    pfmul mm4, mm1

    pfacc mm3, mm4
    add EDX, 16

    pfadd mm3, mm2
    dec ESI

    movd [EDX + -8], mm5
    movq [EDX + -16], mm3

    movq mm3, [EAX]
    movd mm5, [EAX + 8]

    nop
    ja   G3TP2R_2

G3TP2R_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points3_2d_no_rot_raw
_gl_3dnow_transform_points3_2d_no_rot_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TP2NRR_3

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movq mm1, [ECX + 48]
    movq mm4, [EAX]

    movd mm5, [EAX + 8]


ALIGN 32
G3TP2NRR_2:
    add EDX, 16
    pfmul mm4, mm0

    add EAX, EDI
    pfadd mm4, mm1

    dec ESI
    movd [EDX + -8], mm5

    movq [EDX + -16], mm4
    movq mm4, [EAX]

    movd mm5, [EAX + 8]
    ja   G3TP2NRR_2

G3TP2NRR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points3_3d_raw
_gl_3dnow_transform_points3_3d_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TP3R_4

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movq mm2, [ECX + 32]
    movq mm3, [ECX + 48]

    push EAX
    push EDX
    push ESI

    movq mm4, [EAX]
    movd mm6, [EAX + 8]


ALIGN 32
    nop
    nop

G3TP3R_2:
    movq mm5, mm4
    pfmul mm4, mm0

    punpckldq mm6, mm6
    pfmul mm5, mm1

    add EAX, EDI
    pfmul mm6, mm2

    pfacc mm4, mm5
    pfadd mm6, mm3

    pfadd mm6, mm4
    add EDX, 16

    dec ESI
    movq [EDX + -16], mm6

    movq mm4, [EAX]
    movd mm6, [EAX + 8]

    ja   G3TP3R_2

    movd mm0, [ECX + 8]
    punpckldq mm0, [ECX + 24]

    movd mm2, [ECX + 40]
    movd mm3, [ECX + 56]

    pop ESI
    pop EDX
    pop EAX

    movq mm4, [EAX]
    movd mm6, [EAX + 8]


ALIGN 32
G3TP3R_3:
    pfmul mm6, mm2
    movq mm5, mm4

    pfmul mm4, mm0
    pfadd mm6, mm3

    pfmul mm5, mm1
    add EAX, EDI

    pfacc mm4, mm5
    add EDX, 16

    pfadd mm6, mm4
    dec ESI

    movd [EDX + -8], mm6
    movq mm4, [EAX]

    movd mm6, [EAX + 8]
    ja   G3TP3R_3

G3TP3R_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points3_3d_no_rot_raw
_gl_3dnow_transform_points3_3d_no_rot_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TP3NRR_3

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movd mm2, [ECX + 40]
    punpckldq mm2, mm2

    movq mm1, [ECX + 48]
    movd mm3, [ECX + 56]

    punpckldq mm3, mm3
    movq mm4, [EAX]

    movd mm5, [EAX + 8]


ALIGN 32
G3TP3NRR_2:
    pfmul mm4, mm0
    add EAX, EDI

    pfadd mm4, mm1
    pfmul mm5, mm2

    pfadd mm5, mm3
    movq [EDX], mm4

    add EDX, 16
    dec ESI

    movd [EDX + -8], mm5
    movq mm4, [EAX]

    movd mm5, [EAX + 8]
    ja   G3TP3NRR_2

G3TP3NRR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points3_perspective_raw
_gl_3dnow_transform_points3_perspective_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 4
    or byte ptr [ECX + 20], 15
    mov EDX, dword ptr [EAX + 8]
    mov dword ptr [ECX + 8], EDX

ALIGN 32

    push EDI
    push ESI

    mov EDX, dword ptr [ECX + 4]
    mov ECX, ESI
    mov ESI, dword ptr [EAX + 8]
    mov EDI, dword ptr [EAX + 12]
    mov EAX, dword ptr [EAX + 4]

    femms

    cmp ESI, 0
    je   G3TPPR_3

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movq mm1, [ECX + 32]
    movd mm2, [ECX + 40]

    movd mm3, [ECX + 56]

ALIGN 32
    nop
    nop
G3TPPR_2:

    pxor mm7, mm7
    movd mm5, [EAX + 8]

    movq mm4, [EAX]
    add EAX, EDI

    movq mm6, mm5
    pfmul mm4, mm0

    pfsub mm7, mm5
    pfmul mm6, mm2

    punpckldq mm5, mm5
    movd [EDX + 12], mm7

    pfmul mm5, mm1
    pfadd mm6, mm3

    pfadd mm5, mm4
    add EDX, 16

    dec ESI
    movd [EDX + -8], mm6

    movq [EDX + -16], mm5
    ja   G3TPPR_2

G3TPPR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret

CODE32 ENDS

end
