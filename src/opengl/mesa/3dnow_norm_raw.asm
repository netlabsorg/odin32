;
; $Id: 3dnow_norm_raw.asm,v 1.1 2000-03-01 18:49:21 jeroen Exp $
;
; Assembly versions of the transforms
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
PUBLIC _gl_3dnow_transform_normalize_normals_raw
_gl_3dnow_transform_normalize_normals_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EDI, dword ptr [ ESP + 12+16]
    mov EAX, dword ptr [ ESP + 12+12]
    mov EDX, dword ptr [ ESP + 12+24]
    mov EAX, dword ptr [ EAX + 8]
    mov dword ptr [ EDX + 8], EAX
    mov ESI, dword ptr [ ESP + 12+12]
    mov ECX, dword ptr [ ESP + 12+4]
    mov EAX, dword ptr [ EDX + 4]
    mov ECX, dword ptr [ ECX + 64]
    mov EDX, dword ptr [ ESI + 4]


ALIGN 32

    cmp dword ptr [ ESI + 8], 0
    je   G3TN_end

    push EBP
    mov EBP, dword ptr [ ESI + 8]
    femms

    push EBP
    push EAX
    push EDX

    movq mm3, [ ECX + 0 * 4]
    movq mm4, [ ECX + 4 * 4]

    movd mm5, [ ECX + 2 * 4]
    punpckldq mm5, [ ECX + 6 * 4]

    movq mm6, [ ECX + 8 * 4]
    movq mm7, [ ECX + 10 * 4]

    cmp EDI, 0
    jne   G3TN_scale_end

    movd mm0, [ ESP + 28+8]
    punpckldq mm0, mm0

    pfmul mm3, mm0
    pfmul mm4, mm0
    pfmul mm5, mm0
    pfmul mm6, mm0
    pfmul mm7, mm0

G3TN_scale_end:
    movq mm0, [EDX]
    movd mm2, [EDX + 8]

ALIGN 32
G3TN_transform:
    movq mm1, mm0
    punpckldq mm2, mm2

    pfmul mm0, mm3
    add EAX, 12

    pfmul mm1, mm4
    pfacc mm0, mm1

    pfmul mm2, mm5
    pfadd mm0, mm2

    movq mm1, [EDX]
    movq [EAX + -12], mm0

    pfmul mm1, mm6
    movd mm2, [EDX + 8]

    pfmul mm2, mm7
    pfacc mm1, mm1

    pfadd mm1, mm2
    add EDX, dword ptr [ ESI + 12]

    movd [EAX + -4], mm1
    movq mm0, [EDX]

    movd mm2, [EDX + 8]
    dec EBP
    ja   G3TN_transform


    pop EDX
    pop EAX
    pop EBP

    movq mm0, [EAX]
    movd mm1, [EAX + 8]

    cmp EDI, 0
    je   G3TN_norm


ALIGN 32
G3TN_norm_w_lengths:
    movd mm3, [EDI]
    pfmul mm1, mm3

    punpckldq mm3, mm3
    pfmul mm0, mm3

    movq [EAX], mm0
    movd [EAX + 8], mm1

    add EDX, dword ptr [ ESI + 12]
    add EAX, 12

    add EDI, 4
    dec EBP

    movq mm0, [EAX]
    movd mm1, [EAX + 8]
    ja   G3TN_norm_w_lengths
    jmp G3TN_exit_3dnow

ALIGN 32
G3TN_norm:
    movq mm3, mm0
    movq mm4, mm1

    pfmul mm3, mm0
    add EAX, 12

    pfmul mm4, mm1
    pfadd mm3, mm4

    pfacc mm3, mm3
    pfrsqrt mm5, mm3

    movq mm4, mm5
    punpckldq mm3, mm3

    dec EBP
    pfmul mm5, mm5

    pfrsqit1 mm5, mm3
    pfrcpit2 mm5, mm4

    pfmul mm0, mm5

    movq [EAX + -12], mm0
    pfmul mm1, mm5

    movd [EAX + -4], mm1
    movq mm0, [EAX]

    movd mm1, [EAX + 8]
    ja   G3TN_norm

G3TN_exit_3dnow:
    femms
    pop EBP

G3TN_end:
    pop ESI
    pop EDI
    pop ECX
    ret



ALIGN 16

PUBLIC _gl_3dnow_transform_normalize_normals_no_rot_raw
_gl_3dnow_transform_normalize_normals_no_rot_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EDI, dword ptr [ ESP + 12+16]
    mov EAX, dword ptr [ ESP + 12+12]
    mov EDX, dword ptr [ ESP + 12+24]
    mov EAX, dword ptr [ EAX + 8]
    mov dword ptr [ EDX + 8], EAX
    mov ESI, dword ptr [ ESP + 12+12]
    mov ECX, dword ptr [ ESP + 12+4]
    mov EAX, dword ptr [ EDX + 4]
    mov ECX, dword ptr [ ECX + 64]
    mov EDX, dword ptr [ ESI + 4]


ALIGN 32

    cmp dword ptr [ ESI + 8], 0
    je   G3TNNR_end

    push EBP
    mov EBP, dword ptr [ ESI + 8]
    femms

    movd mm0, [ ECX + 0 * 4]
    punpckldq mm0, [ ECX + 5 * 4]

    movd mm2, [ ECX + 10 * 4]
    punpckldq mm2, mm2

    cmp EDI, 0
    jne   G3TNNR_scale_end

    movd mm7, [ ESP + 16+8]
    punpckldq mm7, mm7

    pfmul mm0, mm7
    pfmul mm2, mm7

G3TNNR_scale_end:
    movq mm6, [EDX]
    movd mm7, [EDX + 8]

    cmp EDI, 0
    je   G3TNNR_norm

    movd mm3, [EDI]


G3TNNR_norm_w_lengths:
    pfmul mm6, mm0
    add EDX, dword ptr [ ESI + 12]

    pfmul mm7, mm2
    add EAX, 12

    pfmul mm7, mm3
    punpckldq mm3, mm3

    add EDI, 4
    pfmul mm6, mm3

    dec EBP
    movq [EAX + -12], mm6

    movd [EAX + -4], mm7
    movd mm3, [EDI]

    movq mm6, [EDX]
    movd mm7, [EDX + 8]

    ja   G3TNNR_norm_w_lengths
    jmp G3TNNR_exit_3dnow

G3TNNR_norm:
    pfmul mm6, mm0
    add EAX, 12

    pfmul mm7, mm2
    movq mm3, mm6

    movq mm4, mm7
    pfmul mm3, mm6

    pfmul mm4, mm7
    pfacc mm3, mm3

    pfadd mm3, mm4
    add EDX, dword ptr [ ESI + 12]

    pfrsqrt mm5, mm3
    movq mm4, mm5

    punpckldq mm3, mm3
    pfmul mm5, mm5

    pfrsqit1 mm5, mm3
    dec EBP

    pfrcpit2 mm5, mm4
    pfmul mm6, mm5

    movq [EAX + -12], mm6
    pfmul mm7, mm5

    movd [EAX + -4], mm7
    movq mm6, [EDX]

    movd mm7, [EDX + 8]
    ja   G3TNNR_norm


G3TNNR_exit_3dnow:
    femms
    pop EBP

G3TNNR_end:
    pop ESI
    pop EDI
    pop ECX
    ret






ALIGN 16

PUBLIC _gl_3dnow_transform_rescale_normals_no_rot_raw
_gl_3dnow_transform_rescale_normals_no_rot_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EAX, dword ptr [ ESP + 12+12]
    mov EDX, dword ptr [ ESP + 12+24]
    mov EAX, dword ptr [ EAX + 8]
    mov dword ptr [ EDX + 8], EAX
    mov ESI, dword ptr [ ESP + 12+12]
    mov ECX, dword ptr [ ESP + 12+4]
    mov EAX, dword ptr [ EDX + 4]
    mov ECX, dword ptr [ ECX + 64]
    mov EDX, dword ptr [ ESI + 4]


ALIGN 32

    cmp dword ptr [ ESI + 8], 0
    je   G3TRNR_end

    push EBP
    mov EBP, dword ptr [ ESI + 8]
    femms

    movd mm6, [ ESP + 16+8]
    punpckldq mm6, mm6

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    pfmul mm0, mm6
    movd mm2, [ECX + 40]

    movq mm4, [EDX]
    pfmul mm2, mm6

    movd mm5, [EDX + 8]

ALIGN 32

G3TRNR_rescale:
    pfmul mm4, mm0
    add EDX, dword ptr [ ESI + 12]

    pfmul mm5, mm2
    add EAX, 12

    dec EBP
    movq [EAX + -12], mm4

    movd [EAX + -4], mm5
    movq mm4, [EDX]

    movd mm5, [EDX + 8]
    ja   G3TRNR_rescale

    femms
    pop EBP

G3TRNR_end:
    pop ESI
    pop EDI
    pop ECX
    ret





ALIGN 16
PUBLIC _gl_3dnow_transform_rescale_normals_raw
_gl_3dnow_transform_rescale_normals_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EAX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 36]
    mov EAX, dword ptr [EAX + 8]
    mov dword ptr [EDX + 8], EAX
    mov EDI, dword ptr [ESP + 24]
    mov EAX, dword ptr [EDX + 4]
    mov ECX, dword ptr [ESP + 16]
    mov ESI, dword ptr [ESP + 24]
    mov ECX, dword ptr [ECX + 64]
    mov EDX, dword ptr [EDI + 4]
    mov EDI, dword ptr [EDI + 8]

ALIGN 32

    cmp EDI, 0
    je   G3TR_end

    femms

    movq mm3, [ECX]

    movq mm4, [ECX + 16]
    movd mm0, [ESP + 20]

    movd mm5, [ECX + 8]
    punpckldq mm0, mm0

    punpckldq mm5, [ECX + 24]
    pfmul mm3, mm0

    movq mm6, [ECX + 32]
    pfmul mm4, mm0

    movd mm7, [ECX + 40]
    pfmul mm5, mm0

    pfmul mm6, mm0
    movd mm2, [EDX + 8]

    pfmul mm7, mm0
    movq mm0, [EDX]

G3TR_rescale:
    movq mm1, mm0
    punpckldq mm2, mm2

    pfmul mm0, mm3
    add EAX, 12

    pfmul mm1, mm4
    pfacc mm0, mm1

    pfmul mm2, mm5
    pfadd mm0, mm2

    movq mm1, [EDX]
    movq [EAX + -12], mm0

    pfmul mm1, mm6
    movd mm2, [EDX + 8]

    pfmul mm2, mm7
    pfacc mm1, mm1

    pfadd mm1, mm2
    add EDX, dword ptr [ESI + 12]

    movd [EAX + -4], mm1
    movq mm0, [EDX]

    movd mm2, [EDX + 8]
    dec EDI
    ja   G3TR_rescale

    femms

G3TR_end:
    pop ESI
    pop EDI
    pop ECX
    ret







ALIGN 16
PUBLIC _gl_3dnow_transform_normals_no_rot_raw
_gl_3dnow_transform_normals_no_rot_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EAX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 36]
    mov EAX, dword ptr [EAX + 8]
    mov dword ptr [EDX + 8], EAX
    mov EDI, dword ptr [ESP + 24]
    mov EAX, dword ptr [EDX + 4]
    mov ECX, dword ptr [ESP + 16]
    mov ESI, dword ptr [ESP + 24]
    mov ECX, dword ptr [ECX + 64]
    mov EDX, dword ptr [EDI + 4]
    mov EDI, dword ptr [EDI + 8]


ALIGN 32

    cmp EDI, 0
    je   G3TNR_end

    femms

    movd mm0, [ECX]
    punpckldq mm0, [ECX + 20]

    movd mm2, [ECX + 40]
    punpckldq mm2, mm2

    movq mm4, [EDX]
    movd mm5, [EDX + 8]

ALIGN 32

G3TNR_transform:
    pfmul mm4, mm0
    add EDX, dword ptr [ESI + 12]

    pfmul mm5, mm2
    add EAX, 12

    dec EDI
    movq [EAX + -12], mm4

    movd [EAX + -4], mm5
    movq mm4, [EDX]

    movd mm5, [EDX + 8]
    ja   G3TNR_transform

    femms

G3TNR_end:
    pop ESI
    pop EDI
    pop ECX
    ret








ALIGN 16
PUBLIC _gl_3dnow_transform_normals_raw
_gl_3dnow_transform_normals_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov EAX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 36]
    mov EAX, dword ptr [EAX + 8]
    mov dword ptr [EDX + 8], EAX
    mov EDI, dword ptr [ESP + 24]
    mov EAX, dword ptr [EDX + 4]
    mov ECX, dword ptr [ESP + 16]
    mov ESI, dword ptr [ESP + 24]
    mov ECX, dword ptr [ECX + 64]
    mov EDX, dword ptr [EDI + 4]
    mov EDI, dword ptr [EDI + 8]

ALIGN 32

    cmp EDI, 0
    je   G3T_end

    femms

    movq mm3, [ECX]
    movq mm4, [ECX + 16]

    movd mm5, [ECX + 8]
    punpckldq mm5, [ECX + 24]

    movq mm6, [ECX + 32]
    movd mm7, [ECX + 40]

    movq mm0, [EDX]
    movd mm2, [EDX + 8]

G3T_transform:
    movq mm1, mm0
    punpckldq mm2, mm2

    pfmul mm0, mm3
    add EAX, 12

    pfmul mm1, mm4
    pfacc mm0, mm1

    pfmul mm2, mm5
    pfadd mm0, mm2

    movq mm1, [EDX]
    movq [EAX + -12], mm0

    pfmul mm1, mm6
    movd mm2, [EDX + 8]

    pfmul mm2, mm7
    pfacc mm1, mm1

    pfadd mm1, mm2
    add EDX, dword ptr [ESI + 12]

    movd [EAX + -4], mm1
    movq mm0, [EDX]

    movd mm2, [EDX + 8]
    dec EDI
    ja   G3T_transform

    femms

G3T_end:
    pop ESI
    pop EDI
    pop ECX
    ret






ALIGN 16
PUBLIC _gl_3dnow_normalize_normals_raw
_gl_3dnow_normalize_normals_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov ESI, dword ptr [ESP + 16]
    mov EAX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 36]
    mov EAX, dword ptr [EAX + 8]
    mov dword ptr [EDX + 8], EAX
    mov EAX, dword ptr [EDX + 4]
    mov ECX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 28]
    mov EDI, dword ptr [ESP + 24]
    mov ECX, dword ptr [ECX + 4]


ALIGN 32

    cmp dword ptr [EDI + 8], 0
    je   G3N_end

    push EBP
    mov EBP, dword ptr [EDI + 8]

    femms

    movq mm0, [ECX]
    movd mm1, [ECX + 8]

    cmp EDX, 0
    je   G3N_norm2

G3N_norm1:
    movd mm3, [EDX]
    pfmul mm1, mm3

    punpckldq mm3, mm3
    pfmul mm0, mm3

    movq [EAX], mm0
    movd [EAX + 8], mm1

    add ECX, dword ptr [EDI + 12]
    add EAX, 12

    add EDX, 4
    dec EBP

    movq mm0, [ECX]
    movd mm1, [ECX + 8]
    ja   G3N_norm1

    jmp G3N_end1

G3N_norm2:

    movq mm3, mm0
    add ECX, dword ptr [EDI + 12]

    pfmul mm3, mm0
    movq mm4, mm1

    add EAX, 12
    pfmul mm4, mm1

    pfadd mm3, mm4
    pfacc mm3, mm3

    pfrsqrt mm5, mm3
    movq mm4, mm5

    punpckldq mm3, mm3
    pfmul mm5, mm5

    pfrsqit1 mm5, mm3
    dec EBP

    pfrcpit2 mm5, mm4

    pfmul mm0, mm5
    movq [EAX + -12], mm0

    pfmul mm1, mm5
    movd [EAX + -4], mm1

    movq mm0, [ECX]
    movd mm1, [ECX + 8]
    ja   G3N_norm2

G3N_end1:
    femms
    pop EBP

G3N_end:
    pop ESI
    pop EDI
    pop ECX
    ret






ALIGN 16
PUBLIC _gl_3dnow_rescale_normals_raw
_gl_3dnow_rescale_normals_raw:

    sub ESP, 4
    push EDI
    push ESI

    mov ESI, dword ptr [ESP + 20]
    mov EAX, dword ptr [ESP + 24]
    mov EDX, dword ptr [ESP + 36]
    mov EAX, dword ptr [EAX + 8]
    mov dword ptr [EDX + 8], EAX
    mov EDI, dword ptr [ESP + 24]
    mov ECX, EDI
    mov EAX, dword ptr [EDX + 4]
    mov ECX, dword ptr [ECX + 4]
    mov EDX, dword ptr [EDI + 8]

ALIGN 32

    cmp EDX, 0
    je   G3R_end

    femms

    movd mm0, ESI
    punpckldq mm0, mm0

    movq mm1, [ECX]
    movd mm2, [ECX + 8]

ALIGN 32

G3R_rescale:
    pfmul mm1, mm0
    add ECX, dword ptr [EDI + 12]

    pfmul mm2, mm0
    add EAX, 12

    dec EDX
    movq [EAX + -12], mm1

    movd [EAX + -4], mm2
    movq mm1, [ECX]

    movd mm2, [ECX + 8]
    ja   G3R_rescale

    femms

G3R_end:
    pop ESI
    pop EDI
    pop ECX
    ret

CODE32 ENDS

end
