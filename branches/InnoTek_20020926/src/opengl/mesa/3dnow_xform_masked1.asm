; $Id: 3dnow_xform_masked1.asm,v 1.1 2000-03-01 18:49:21 jeroen Exp $
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
PUBLIC _gl_3dnow_transform_points1_general_masked
_gl_3dnow_transform_points1_general_masked:

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

    movq mm0, [ECX]
    movq mm1, [ECX + 8]

    movq mm2, [ECX + 48]
    movq mm3, [ECX + 56]

    cmp ESI, 0
    je   G3TPGM_4


ALIGN 32
G3TPGM_2:
    test byte ptr [EBP], BL
    jnz   G3TPGM_3

    movd mm4, [EAX]
    punpckldq mm4, mm4

    movq mm5, mm4
    pfmul mm4, mm0

    pfmul mm5, mm1
    pfadd mm4, mm2

    pfadd mm5, mm3
    movq [EDX], mm4

    movq [EDX + 8], mm5

G3TPGM_3:
    add EAX, EDI

    inc EBP
    add EDX, 16

    dec ESI
    ja   G3TPGM_2

G3TPGM_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points1_identity_masked
_gl_3dnow_transform_points1_identity_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 1
    or byte ptr [ECX + 20], 1
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

    movd mm0, [EAX]
    movd [EDX], mm0

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
PUBLIC _gl_3dnow_transform_points1_2d_masked
_gl_3dnow_transform_points1_2d_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 2
    or byte ptr [ECX + 20], 3
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

    movq mm0, [ECX]
    movq mm2, [ECX + 48]

    cmp ESI, 0
    je   G3TP2M_4


ALIGN 32

G3TP2M_2:
    test byte ptr [EBP], BL
    jnz   G3TP2M_3

    movd mm4, [EAX]
    punpckldq mm4, mm4

    pfmul mm4, mm0
    pfadd mm4, mm2

    movq [EDX], mm4

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
PUBLIC _gl_3dnow_transform_points1_2d_no_rot_masked
_gl_3dnow_transform_points1_2d_no_rot_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 2
    or byte ptr [ECX + 20], 3
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
    movq mm2, [ECX + 48]

    cmp ESI, 0
    je   G3TP2NRM_4


ALIGN 32

G3TP2NRM_2:
    test byte ptr [EBP], BL
    jnz   G3TP2NRM_3

    movd mm4, [EAX]
    pfmul mm4, mm0

    pfadd mm4, mm2
    movq [EDX], mm4

G3TP2NRM_3:
    add EDX, 16
    add EAX, EDI

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
PUBLIC _gl_3dnow_transform_points1_3d_masked
_gl_3dnow_transform_points1_3d_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
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

    movq mm0, [ECX]
    movd mm1, [ECX + 8]

    movq mm2, [ECX + 48]
    movd mm3, [ECX + 56]

    cmp ESI, 0
    je   G3TP3M_4

ALIGN 32

G3TP3M_2:
    test byte ptr [EBP], BL
    jnz   G3TP3M_3

    movd mm4, [EAX]
    punpckldq mm4, mm4

    movq mm5, mm4
    pfmul mm4, mm0

    pfmul mm5, mm1
    pfadd mm4, mm2

    pfadd mm5, mm3
    movq [EDX], mm4

    movd [EDX + 8], mm5

G3TP3M_3:
    add EAX, EDI

    add EDX, 16
    inc EBP

    dec ESI
    ja   G3TP3M_2

G3TP3M_4:
    femms

    pop EBP
    pop EBX
    pop EDI
    pop ESI

    pop ESI
    ret




ALIGN 16
PUBLIC _gl_3dnow_transform_points1_3d_no_rot_masked
_gl_3dnow_transform_points1_3d_no_rot_masked:

    push ESI
    mov ECX, dword ptr [ESP + 8]
    mov ESI, dword ptr [ESP + 12]
    mov EAX, dword ptr [ESP + 16]
    mov dword ptr [ECX + 16], 3
    or byte ptr [ECX + 20], 7
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
    movq mm2, [ECX + 48]

    movd mm3, [ECX + 56]
    cmp ESI, 0
    je   G3TP3NRM_4

ALIGN 32

G3TP3NRM_2:
    test byte ptr [EBP], BL
    jnz   G3TP3NRM_3

    movd mm4, [EAX]
    pfmul mm4, mm0

    pfadd mm4, mm2
    movq [EDX], mm4

    movd [EDX + 8], mm3

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
PUBLIC _gl_3dnow_transform_points1_perspective_masked
_gl_3dnow_transform_points1_perspective_masked:

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
    movd mm3, [ECX + 56]

    cmp ESI, 0
    je   G3TPPM_4

ALIGN 32

G3TPPM_2:
    test byte ptr [EBP], BL
    jnz   G3TPPM_3

    movd mm4, [EAX]
    pfmul mm4, mm0

    movq [EDX], mm4
    movq [EDX + 8], mm3

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
