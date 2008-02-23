; $Id: 3dnow_xform_raw4.asm,v 1.1 2000-03-01 18:49:22 jeroen Exp $
;
; 3dNow! transforms
;

       .586p
       .MMX

include amd3d.inc

CODE32 SEGMENT PAGE USE32 PUBLIC 'CODE'
CODE32 ENDS
DATA32 SEGMENT PAGE USE32 PUBLIC 'DATA'
ASSUME DS:FLAT, SS:FLAT, ES:FLAT
DATA32 ENDS

CODE32 SEGMENT
ALIGN 16
PUBLIC _gl_3dnow_transform_points4_general_raw
_gl_3dnow_transform_points4_general_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movd mm2, [ECX + 32]
    punpckldq mm2, [ECX + 48]

    movd mm3, [ECX + 36]
    punpckldq mm3, [ECX + 52]

    cmp ESI, 0
    je   G3TPGR_4

    push EAX
    push EDX
    push ESI


ALIGN 32
G3TPGR_2:

    movq mm4, [EAX]
    movq mm5, mm4

    movq mm6, [EAX + 8]
    pfmul _mm4, _mm0

    movq mm7, mm6
    pfmul mm5, mm1

    pfmul mm6, mm2
    pfacc mm4, mm5

    pfmul mm7, mm3
    add EAX, EDI

    pfacc mm6, mm7
    pfadd mm6, mm4

    movq [EDX], mm6
    add EDX, 16

    dec ESI
    ja   G3TPGR_2

    movd mm0, [ECX + 8]
    punpckldq mm0, [ECX + 24]

    movd mm1, [ECX + 12]
    punpckldq mm1, [ECX + 28]

    movd mm2, [ECX + 40]
    punpckldq mm2, [ECX + 56]

    movd mm3, [ECX + 44]
    punpckldq mm3, [ECX + 60]

    pop ESI
    pop EDX

    pop EAX


ALIGN 32
G3TPGR_3:

    movq mm4, [EAX]
    movq mm5, mm4

    movq mm6, [EAX + 8]
    pfmul mm4, mm0

    movq mm7, mm6
    pfmul mm5, mm1

    pfmul mm6, mm2
    pfacc mm4, mm5

    pfmul mm7, mm3
    add EAX, EDI

    pfacc mm6, mm7
    pfadd mm6, mm4

    movq [EDX + 8], mm6
    add EDX, 16

    dec ESI
    ja   G3TPGR_3

G3TPGR_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_identity_raw
_gl_3dnow_transform_points4_identity_raw:

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

G3TPIR_3:
    movq mm0, [EAX]
    movq mm1, [EAX + 8]

    add EAX, EDI
    movq [EDX], mm0

    movq [EDX + 8], mm1
    add EDX, 16

    dec ESI
    ja   G3TPIR_3

G3TPIR_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_2d_raw
_gl_3dnow_transform_points4_2d_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movq mm2, [ECX + 48]
    cmp ESI, 0
    je   G3TP2R_3


ALIGN 32
G3TP2R_2:

    movq mm3, [EAX]
    movq mm4, mm3

    movq mm5, [EAX + 8]
    pfmul mm3, mm0

    movq mm6, mm5
    pfmul mm4, mm1

    punpckhdq mm6, mm6
    add EAX, EDI

    pfmul mm6, mm2
    pfacc mm3, mm4

    pfadd mm3, mm6
    movq [EDX + 8], mm5

    movq [EDX], mm3
    add EDX, 16

    dec ESI
    ja   G3TP2R_2

G3TP2R_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_2d_no_rot_raw
_gl_3dnow_transform_points4_2d_no_rot_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movq mm1, [ECX + 48]
    cmp ESI, 0
    je   G3TP2NRR_3


ALIGN 32
G3TP2NRR_2:

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    pfmul mm4, mm0
    movq mm6, mm5

    punpckhdq mm6, mm6
    add EAX, EDI

    pfmul mm6, mm1
    movq [EDX + 8], mm5

    pfadd mm6, mm4
    movq [EDX], mm6

    add EDX, 16
    dec ESI

    ja   G3TP2NRR_2

G3TP2NRR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_3d_raw
_gl_3dnow_transform_points4_3d_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 16]

    movd mm1, [ECX + 4]
    punpckldq mm1, [ECX + 20]

    movd mm2, [ECX + 32]
    punpckldq mm2, [ECX + 48]

    movd mm3, [ECX + 36]
    punpckldq mm3, [ECX + 52]

    cmp ESI, 0
    je   G3TP3R_4

    push EAX
    push EDX
    push ESI


ALIGN 32
G3TP3R_2:
    movq mm4, [EAX]
    movq mm5, mm4

    pfmul mm4, mm0
    movq mm6, [EAX + 8]

    pfmul mm5, mm1
    movq mm7, mm6

    pfacc mm4, mm5
    pfmul mm6, mm2

    pfmul mm7, mm3
    add EAX, EDI

    pfacc mm6, mm7
    pfadd mm6, mm4

    movq [EDX], mm6
    add EDX, 16

    dec ESI
    ja   G3TP3R_2

    movd mm0, [ECX + 8]
    punpckldq mm0, [ECX + 24]

    movd mm2, [ECX + 40]
    punpckldq mm2, [ECX + 56]

    pop ESI
    pop EDX

    pop EAX


ALIGN 32
G3TP3R_3:
    movq mm4, [EAX]
    movq mm6, [EAX + 8]

    movd mm7, [EAX + 12]
    pfmul mm4, mm0

    pfmul mm6, mm2
    add EAX, EDI

    pfacc mm6, mm4
    pfacc mm6, mm7

    movq [EDX + 8], mm6
    add EDX, 16

    dec ESI
    ja   G3TP3R_3

G3TP3R_4:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points4_3d_no_rot_raw
_gl_3dnow_transform_points4_3d_no_rot_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movd mm2, [ECX + 40]
    punpckldq mm2, [ECX + 56]

    movq mm1, [ECX + 48]
    cmp ESI, 0

    je   G3TP3NRR_3


ALIGN 32
G3TP3NRR_2:

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    movq mm6, mm5
    pfmul mm4, mm0

    movd mm7, [EAX + 12]
    pfmul mm5, mm2

    punpckhdq mm6, mm6
    pfacc mm5, mm7

    pfmul mm6, mm1
    add EAX, EDI

    pfadd mm4, mm6
    movq [EDX + 8], mm5

    movq [EDX], mm4
    add EDX, 16

    dec ESI
    ja   G3TP3NRR_2

G3TP3NRR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points4_perspective_raw
_gl_3dnow_transform_points4_perspective_raw:

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

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movd mm1, [ECX + 40]
    punpckldq mm1, [ECX + 56]

    movq mm2, [ECX + 32]
    pxor mm7, mm7

    cmp ESI, 0
    je   G3TPPR_3


ALIGN 32
G3TPPR_2:

    movq mm4, [EAX]
    movq mm5, [EAX + 8]

    pfmul mm4, mm0
    movq mm6, mm5

    punpckldq mm5, mm5
    pfmul mm5, mm2

    pfadd mm5, mm4
    movq [EDX], mm5

    movd mm5, [EAX + 8]
    pfmul mm6, mm1

    pfsubr mm5, mm7
    add EAX, EDI

    pfacc mm6, mm5
    movq [EDX + 8], mm6

    add EDX, 16
    dec ESI

    ja   G3TPPR_2

G3TPPR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret

CODE32 ENDS

end
