; $Id: 3dnow_xform_raw1.asm,v 1.1 2000-03-01 18:49:21 jeroen Exp $
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
PUBLIC _gl_3dnow_transform_points1_general_raw
_gl_3dnow_transform_points1_general_raw:

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
    je   G3TPGR_3

    movq mm0, [ECX]
    movq mm1, [ECX + 8]

    movq mm2, [ECX + 48]
    movq mm3, [ECX + 56]

ALIGN 32
G3TPGR_2:

    movd mm4, [EAX]
    punpckldq mm4, mm4

    movq mm5, mm4
    pfmul mm4, mm0

    pfmul mm5, mm1
    pfadd mm4, mm2

    pfadd mm5, mm3
    movq [EDX], mm4

    movq [EDX + 8], mm5
    add EAX, EDI

    add EDX, 16
    dec ESI

    ja   G3TPGR_2

G3TPGR_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points1_identity_raw
_gl_3dnow_transform_points1_identity_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 1
    or byte ptr [ECX + 20], 1
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

G3TPIR_3:

    movd mm0, [EAX]
    add EAX, EDI

    movd [EDX], mm0
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
PUBLIC _gl_3dnow_transform_points1_2d_raw
_gl_3dnow_transform_points1_2d_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 2
    or byte ptr [ECX + 20], 3
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

    movq mm0, [ECX]
    movq mm2, [ECX + 48]


ALIGN 32
G3TP2R_2:

    movd mm4, [EAX]
    punpckldq mm4, mm4

    pfmul mm4, mm0
    pfadd mm4, mm2

    movq [EDX], mm4
    add EAX, EDI

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
PUBLIC _gl_3dnow_transform_points1_2d_no_rot_raw
_gl_3dnow_transform_points1_2d_no_rot_raw:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 2
    or byte ptr [ECX + 20], 3
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
    movq mm2, [ECX + 48]


ALIGN 32
G3TP2NRR_2:

    movd mm4, [EAX]
    add EAX, EDI

    pfmul mm4, mm0
    pfadd mm4, mm2

    movq [EDX], mm4
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
PUBLIC _gl_3dnow_transform_points1_3d_raw
_gl_3dnow_transform_points1_3d_raw:

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
    je   G3TP3R_3

    movq mm0, [ECX]
    movd mm1, [ECX + 8]

    movq mm2, [ECX + 48]
    movd mm3, [ECX + 56]


ALIGN 32
G3TP3R_2:

    movd mm4, [EAX]
    punpckldq mm4, mm4

    movq mm5, mm4
    pfmul mm4, mm0

    pfmul mm5, mm1
    pfadd mm4, mm2

    pfadd mm5, mm3
    movq [EDX], mm4

    movd [EDX + 8], mm5
    add EAX, EDI

    add EDX, 16
    dec ESI

    ja   G3TP3R_2

G3TP3R_3:
    femms

    pop ESI
    pop EDI

    pop ESI
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_points1_3d_no_rot_raw
_gl_3dnow_transform_points1_3d_no_rot_raw:

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
    movq mm2, [ECX + 48]

    movd mm3, [ECX + 56]

ALIGN 32
G3TP3NRR_2:

    movd mm4, [EAX]
    pfmul mm4, mm0

    pfadd mm4, mm2
    movq [EDX], mm4

    movd [EDX + 8], mm3
    add EAX, EDI

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
PUBLIC _gl_3dnow_transform_points1_perspective_raw
_gl_3dnow_transform_points1_perspective_raw:

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
    movd mm3, [ECX + 56]

ALIGN 32
G3TPPR_2:
    movd mm4, [EAX]
    pfmul mm4, mm0

    movq [EDX], mm4
    movq [EDX + 8], mm3

    add EAX, EDI
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
