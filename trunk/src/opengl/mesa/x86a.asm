; $Id: x86a.asm,v 1.1 2000-03-01 18:49:41 jeroen Exp $
;
; Assembly versions of the transforms
;

        .586p

CODE32 SEGMENT PARA USE32 PUBLIC 'CODE'
CODE32 ENDS
DATA32 SEGMENT PARA USE32 PUBLIC 'DATA'
ASSUME DS:FLAT, SS:FLAT, ES:FLAT
DATA32 ENDS

CODE32 SEGMENT
        PUBLIC _gl_x86_transform_points2_general_masked
        ALIGN 16
_gl_x86_transform_points2_general_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2mgm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p2mgm_top:

        test byte ptr [EBP], BL
        jnz   p2mgm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p2mgm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2mgm_top


p2mgm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_identity_masked
        ALIGN 4
_gl_x86_transform_points2_identity_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2mim_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        cmp EDI, ESI
        je   p2mim_finish

        ALIGN 4
p2mim_top:

        test byte ptr [EBP], BL
        jnz   p2mim_skip

        mov EAX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EAX
        mov dword ptr [ EDI + 1 * 4], EDX
p2mim_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p2mim_top


p2mim_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_masked
        ALIGN 4
_gl_x86_transform_points2_2d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2m2dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p2m2dm_top:

        test byte ptr [EBP], BL
        jnz   p2m2dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

p2m2dm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m2dm_top


p2m2dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points2_2d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2m2dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p2m2dnrm_top:

        test byte ptr [EBP], BL
        jnz   p2m2dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

p2m2dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m2dnrm_top


p2m2dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_masked
        ALIGN 4
_gl_x86_transform_points2_3d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2m3dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p2m3dm_top:

        test byte ptr [EBP], BL
        jnz   p2m3dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p2m3dm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m3dm_top


p2m3dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points2_3d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2m3dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        mov EAX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2m3dnrm_top:

        test byte ptr [EBP], BL
        jnz   p2m3dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX

p2m3dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p2m3dnrm_top


p2m3dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_perspective_masked
        ALIGN 4
_gl_x86_transform_points2_perspective_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p2mpm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        mov EAX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2mpm_top:

        test byte ptr [EBP], BL
        jnz   p2mpm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX
        mov dword ptr [ EDI + 3 * 4], 0

p2mpm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p2mpm_top


p2mpm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_general_masked
        ALIGN 4
_gl_x86_transform_points3_general_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3mgm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p3mgm_top:

        test byte ptr [EBP], BL
        jnz   p3mgm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p3mgm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3mgm_top


p3mgm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_identity_masked
        ALIGN 4
_gl_x86_transform_points3_identity_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3mim_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX
        mov dword ptr [ ESP + 16+4], ECX


        cmp EDI, ESI
        je   p3mim_finish

        ALIGN 4
p3mim_top:

        test byte ptr [EBP], BL
        jnz   p3mim_skip

        mov EAX, dword ptr [ ESI + 0 * 4]
        mov ECX, dword ptr [ ESI + 1 * 4]
        mov EDX, dword ptr [ ESI + 2 * 4]

        mov dword ptr [ EDI + 0 * 4], EAX
        mov dword ptr [ EDI + 1 * 4], ECX
        mov dword ptr [ EDI + 2 * 4], EDX
p3mim_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, dword ptr [ ESP + 16+4]
        jne   p3mim_top


p3mim_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_masked
        ALIGN 4
_gl_x86_transform_points3_2d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3m2dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p3m2dm_top:

        test byte ptr [EBP], BL
        jnz   p3m2dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        mov EAX, dword ptr [ ESI + 2 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX

p3m2dm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p3m2dm_top


p3m2dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points3_2d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3m2dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p3m2dnrm_top:

        test byte ptr [EBP], BL
        jnz   p3m2dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]

        fxch ST(2)
        faddp ST(2), ST(0)

        mov EAX, dword ptr [ ESI + 2 * 4]

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX

p3m2dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p3m2dnrm_top


p3m2dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_masked
        ALIGN 4
_gl_x86_transform_points3_3d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3m3dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p3m3dm_top:

        test byte ptr [EBP], BL
        jnz   p3m3dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p3m3dm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m3dm_top


p3m3dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points3_3d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3m3dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p3m3dnrm_top:

        test byte ptr [EBP], BL
        jnz   p3m3dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        faddp ST(3), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p3m3dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m3dnrm_top


p3m3dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_perspective_masked
        ALIGN 4
_gl_x86_transform_points3_perspective_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3mpm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p3mpm_top:

        test byte ptr [EBP], BL
        jnz   p3mpm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(1)
        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 2 * 4]
        xor EAX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EAX

p3mpm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p3mpm_top


p3mpm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_general_masked
        ALIGN 4
_gl_x86_transform_points4_general_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4mgm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI



        ALIGN 4
p4mgm_top:

        test byte ptr [EBP], BL
        jnz   p4mgm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 15 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p4mgm_skip:
        inc EBP
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4mgm_top


p4mgm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_identity_masked
        ALIGN 4
_gl_x86_transform_points4_identity_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4mim_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        cmp EDI, ESI
        je   p4mim_finish

        ALIGN 4
p4mim_top:

        test byte ptr [EBP], BL
        jnz   p4mim_skip

        mov EAX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EAX
        mov dword ptr [ EDI + 1 * 4], EDX

        mov EAX, dword ptr [ ESI + 2 * 4]
        mov EDX, dword ptr [ ESI + 3 * 4]

        mov dword ptr [ EDI + 2 * 4], EAX
        mov dword ptr [ EDI + 3 * 4], EDX
p4mim_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p4mim_top


p4mim_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_masked
        ALIGN 4
_gl_x86_transform_points4_2d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX
        mov dword ptr [ ESP + 16+4], ECX


        ALIGN 4
p4m2dm_top:

        test byte ptr [EBP], BL
        jnz   p4m2dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 2 * 4]
        mov ECX, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX
        mov dword ptr [ EDI + 3 * 4], ECX

p4m2dm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, dword ptr [ ESP + 16+4]
        jne   p4m2dm_top


p4m2dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points4_2d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX
        mov dword ptr [ ESP + 16+4], ECX


        ALIGN 4
p4m2dnrm_top:

        test byte ptr [EBP], BL
        jnz   p4m2dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 2 * 4]
        mov ECX, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EAX
        mov dword ptr [ EDI + 3 * 4], ECX

p4m2dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, dword ptr [ ESP + 16+4]
        jne   p4m2dnrm_top


p4m2dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_masked
        ALIGN 4
_gl_x86_transform_points4_3d_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m3dm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p4m3dm_top:

        test byte ptr [EBP], BL
        jnz   p4m3dm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EAX

p4m3dm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p4m3dm_top


p4m3dm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_no_rot_masked
        ALIGN 4
_gl_x86_transform_points4_3d_no_rot_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m3dnrm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p4m3dnrm_top:

        test byte ptr [EBP], BL
        jnz   p4m3dnrm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EAX

p4m3dnrm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p4m3dnrm_top


p4m3dnrm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_perspective_masked
        ALIGN 4
_gl_x86_transform_points4_perspective_masked:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov EBP, dword ptr [ ESP + 16+16]

        mov ECX, dword ptr [ ESI + 8]
        mov BL, byte ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4mpm_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI

        mov dword ptr [ ESP + 16+12], EAX


        ALIGN 4
p4mpm_top:

        test byte ptr [EBP], BL
        jnz   p4mpm_skip


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        faddp ST(1), ST(0)

        mov EAX, dword ptr [ ESI + 2 * 4]
        xor EAX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EAX

p4mpm_skip:
        inc EBP
        add EDI, 16
        add ESI, dword ptr [ ESP + 16+12]
        cmp EDI, ECX
        jne   p4mpm_top


p4mpm_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret






        PUBLIC _gl_x86_transform_points2_general_raw
        ALIGN 4
_gl_x86_transform_points2_general_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2mgr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p2mgr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p2mgr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2mgr_top


p2mgr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_identity_raw
        ALIGN 4
_gl_x86_transform_points2_identity_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2mir_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        cmp EDI, ESI
        je   p2mir_finish

        ALIGN 4
p2mir_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX
p2mir_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2mir_top


p2mir_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_raw
        ALIGN 4
_gl_x86_transform_points2_2d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2m2dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p2m2dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

p2m2dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m2dr_top


p2m2dr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points2_2d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2m2dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 3

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 2

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p2m2dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

p2m2dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m2dnrr_top


p2m2dnrr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_raw
        ALIGN 4
_gl_x86_transform_points2_3d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2m3dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p2m3dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p2m3dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m3dr_top


p2m3dr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points2_3d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2m3dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2m3dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

p2m3dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2m3dnrr_top


p2m3dnrr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_perspective_raw
        ALIGN 4
_gl_x86_transform_points2_perspective_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p2mpr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2mpr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], 0

p2mpr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p2mpr_top


p2mpr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_general_raw
        ALIGN 4
_gl_x86_transform_points3_general_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3mgr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3mgr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p3mgr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3mgr_top


p3mgr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_identity_raw
        ALIGN 4
_gl_x86_transform_points3_identity_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX
        push EBP

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3mir_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        cmp EDI, ESI
        je   p3mir_finish

        ALIGN 4
p3mir_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EBP, dword ptr [ ESI + 1 * 4]
        mov EDX, dword ptr [ ESI + 2 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EBP
        mov dword ptr [ EDI + 2 * 4], EDX
p3mir_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3mir_top


p3mir_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_raw
        ALIGN 4
_gl_x86_transform_points3_2d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3m2dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3m2dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        mov EBX, dword ptr [ ESI + 2 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

p3m2dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m2dr_top


p3m2dr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points3_2d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3m2dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3m2dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]

        fxch ST(2)
        faddp ST(2), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

p3m2dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m2dnrr_top


p3m2dnrr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_raw
        ALIGN 4
_gl_x86_transform_points3_3d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3m3dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3m3dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p3m3dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m3dr_top


p3m3dr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points3_3d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3m3dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 7

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 3

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3m3dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        faddp ST(3), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

p3m3dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3m3dnrr_top


p3m3dnrr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_perspective_raw
        ALIGN 4
_gl_x86_transform_points3_perspective_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p3mpr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p3mpr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(1)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p3mpr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p3mpr_top


p3mpr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_general_raw
        ALIGN 4
_gl_x86_transform_points4_general_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]


        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4mgr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4mgr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 15 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p4mgr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4mgr_top


p4mgr_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_identity_raw
        ALIGN 4
_gl_x86_transform_points4_identity_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4mir_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        cmp EDI, ESI
        je   p4mir_finish

        ALIGN 4
p4mir_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EDX, dword ptr [ ESI + 3 * 4]

        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EDX
p4mir_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4mir_top


p4mir_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_raw
        ALIGN 4
_gl_x86_transform_points4_2d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX
        push EBP

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4m2dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4m2dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4m2dr_top


p4m2dr_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points4_2d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX
        push EBP

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4m2dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4m2dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4m2dnrr_top


p4m2dnrr_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_raw
        ALIGN 4
_gl_x86_transform_points4_3d_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4m3dr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4m3dr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4m3dr_top


p4m3dr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_no_rot_raw
        ALIGN 4
_gl_x86_transform_points4_3d_no_rot_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4m3dnrr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4m3dnrr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dnrr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4m3dnrr_top


p4m3dnrr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_perspective_raw
        ALIGN 4
_gl_x86_transform_points4_perspective_raw:

        push ESI
        push EDI

        mov ESI, dword ptr [ ESP + 12+8]
        mov EDI, dword ptr [ ESP + 4+8]

        push EBX

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESI + 8]

        test ECX, ECX
        jz   p4mpr_finish

        mov EAX, dword ptr [ ESI + 12]
        or dword ptr [ EDI + 20], 15

        mov dword ptr [ EDI + 8], ECX
        mov dword ptr [ EDI + 16], 4

        shl ECX, 4
        mov ESI, dword ptr [ ESI + 4]

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDI


        ALIGN 4
p4mpr_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4mpr_skip:
        add EDI, 16
        add ESI, EAX
        cmp EDI, ECX
        jne   p4mpr_top


p4mpr_finish:
        pop EBX
        pop EDI
        pop ESI
        ret











        PUBLIC _gl_x86_transform_points2_general_v8
        ALIGN 4
_gl_x86_transform_points2_general_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2mgv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2mgv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p2mgv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2mgv8_top


p2mgv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_identity_v8
        ALIGN 4
_gl_x86_transform_points2_identity_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2miv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p2miv8_finish

        ALIGN 4
p2miv8_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX
        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2miv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2miv8_top


p2miv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_v8
        ALIGN 4
_gl_x86_transform_points2_2d_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2m2dv8_top


p2m2dv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points2_2d_no_rot_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dnrv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2m2dnrv8_top


p2m2dnrv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_v8
        ALIGN 4
_gl_x86_transform_points2_3d_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m3dv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m3dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2m3dv8_top


p2m3dv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points2_3d_no_rot_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2m3dnrv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2m3dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2m3dnrv8_top


p2m3dnrv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_perspective_v8
        ALIGN 4
_gl_x86_transform_points2_perspective_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2mpv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2mpv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], 0

p2mpv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p2mpv8_top


p2mpv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_general_v8
        ALIGN 4
_gl_x86_transform_points3_general_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3mgv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3mgv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p3mgv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3mgv8_top


p3mgv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_identity_v8
        ALIGN 4
_gl_x86_transform_points3_identity_v8:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3miv8_finish

        mov EAX, dword ptr [ ESP + 16+16]


        cmp EDI, ESI
        je   p3miv8_finish

        ALIGN 4
p3miv8_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EBP, dword ptr [ ESI + 1 * 4]
        mov EDX, dword ptr [ ESI + 2 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EBP
        mov dword ptr [ EDI + 2 * 4], EDX
        mov dword ptr [ EDI + 3 * 4], 1065353216
p3miv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3miv8_top


p3miv8_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_v8
        ALIGN 4
_gl_x86_transform_points3_2d_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        mov EBX, dword ptr [ ESI + 2 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3m2dv8_top


p3m2dv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points3_2d_no_rot_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dnrv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]

        fxch ST(2)
        faddp ST(2), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3m2dnrv8_top


p3m2dnrv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_v8
        ALIGN 4
_gl_x86_transform_points3_3d_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3m3dv8_top


p3m3dv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points3_3d_no_rot_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dnrv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        faddp ST(3), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3m3dnrv8_top


p3m3dnrv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_perspective_v8
        ALIGN 4
_gl_x86_transform_points3_perspective_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3mpv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3mpv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(1)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p3mpv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p3mpv8_top


p3mpv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_general_v8
        ALIGN 4
_gl_x86_transform_points4_general_v8:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p4mgv8_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p4mgv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 15 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p4mgv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4mgv8_top


p4mgv8_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_identity_v8
        ALIGN 4
_gl_x86_transform_points4_identity_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4miv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p4miv8_finish

        ALIGN 4
p4miv8_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EDX, dword ptr [ ESI + 3 * 4]

        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EDX
p4miv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4miv8_top


p4miv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_v8
        ALIGN 4
_gl_x86_transform_points4_2d_v8:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dv8_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4m2dv8_top


p4m2dv8_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points4_2d_no_rot_v8:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dnrv8_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4m2dnrv8_top


p4m2dnrv8_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_v8
        ALIGN 4
_gl_x86_transform_points4_3d_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4m3dv8_top


p4m3dv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_no_rot_v8
        ALIGN 4
_gl_x86_transform_points4_3d_no_rot_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dnrv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dnrv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dnrv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4m3dnrv8_top


p4m3dnrv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_perspective_v8
        ALIGN 4
_gl_x86_transform_points4_perspective_v8:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4mpv8_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4mpv8_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4mpv8_skip:
        add EDI, 32
        add ESI, EAX
        dec ECX
        jnz   p4mpv8_top


p4mpv8_finish:
        pop EBX
        pop EDI
        pop ESI
        ret




        PUBLIC _gl_x86_transform_points2_general_v12
        ALIGN 4
_gl_x86_transform_points2_general_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2mgv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2mgv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p2mgv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2mgv12_top


p2mgv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_identity_v12
        ALIGN 4
_gl_x86_transform_points2_identity_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2miv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p2miv12_finish

        ALIGN 4
p2miv12_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX
        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2miv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2miv12_top


p2miv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_v12
        ALIGN 4
_gl_x86_transform_points2_2d_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2m2dv12_top


p2m2dv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points2_2d_no_rot_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dnrv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2m2dnrv12_top


p2m2dnrv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_v12
        ALIGN 4
_gl_x86_transform_points2_3d_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m3dv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m3dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2m3dv12_top


p2m3dv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points2_3d_no_rot_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2m3dnrv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2m3dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2m3dnrv12_top


p2m3dnrv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_perspective_v12
        ALIGN 4
_gl_x86_transform_points2_perspective_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2mpv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2mpv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], 0

p2mpv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p2mpv12_top


p2mpv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_general_v12
        ALIGN 4
_gl_x86_transform_points3_general_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3mgv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3mgv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p3mgv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3mgv12_top


p3mgv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_identity_v12
        ALIGN 4
_gl_x86_transform_points3_identity_v12:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3miv12_finish

        mov EAX, dword ptr [ ESP + 16+16]


        cmp EDI, ESI
        je   p3miv12_finish

        ALIGN 4
p3miv12_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EBP, dword ptr [ ESI + 1 * 4]
        mov EDX, dword ptr [ ESI + 2 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EBP
        mov dword ptr [ EDI + 2 * 4], EDX
        mov dword ptr [ EDI + 3 * 4], 1065353216
p3miv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3miv12_top


p3miv12_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_v12
        ALIGN 4
_gl_x86_transform_points3_2d_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        mov EBX, dword ptr [ ESI + 2 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3m2dv12_top


p3m2dv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points3_2d_no_rot_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dnrv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]

        fxch ST(2)
        faddp ST(2), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3m2dnrv12_top


p3m2dnrv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_v12
        ALIGN 4
_gl_x86_transform_points3_3d_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3m3dv12_top


p3m3dv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points3_3d_no_rot_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dnrv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        faddp ST(3), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3m3dnrv12_top


p3m3dnrv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_perspective_v12
        ALIGN 4
_gl_x86_transform_points3_perspective_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3mpv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3mpv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(1)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p3mpv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p3mpv12_top


p3mpv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_general_v12
        ALIGN 4
_gl_x86_transform_points4_general_v12:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p4mgv12_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p4mgv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 15 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p4mgv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4mgv12_top


p4mgv12_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_identity_v12
        ALIGN 4
_gl_x86_transform_points4_identity_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4miv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p4miv12_finish

        ALIGN 4
p4miv12_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EDX, dword ptr [ ESI + 3 * 4]

        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EDX
p4miv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4miv12_top


p4miv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_v12
        ALIGN 4
_gl_x86_transform_points4_2d_v12:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dv12_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4m2dv12_top


p4m2dv12_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points4_2d_no_rot_v12:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dnrv12_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4m2dnrv12_top


p4m2dnrv12_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_v12
        ALIGN 4
_gl_x86_transform_points4_3d_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4m3dv12_top


p4m3dv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_no_rot_v12
        ALIGN 4
_gl_x86_transform_points4_3d_no_rot_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dnrv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dnrv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dnrv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4m3dnrv12_top


p4m3dnrv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_perspective_v12
        ALIGN 4
_gl_x86_transform_points4_perspective_v12:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4mpv12_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4mpv12_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4mpv12_skip:
        add EDI, 48
        add ESI, EAX
        dec ECX
        jnz   p4mpv12_top


p4mpv12_finish:
        pop EBX
        pop EDI
        pop ESI
        ret




        PUBLIC _gl_x86_transform_points2_general_v16
        ALIGN 4
_gl_x86_transform_points2_general_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2mgv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2mgv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p2mgv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2mgv16_top


p2mgv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_identity_v16
        ALIGN 4
_gl_x86_transform_points2_identity_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2miv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p2miv16_finish

        ALIGN 4
p2miv16_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX
        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2miv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2miv16_top


p2miv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_v16
        ALIGN 4
_gl_x86_transform_points2_2d_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2m2dv16_top


p2m2dv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_2d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points2_2d_no_rot_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m2dnrv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m2dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]

        mov dword ptr [ EDI + 2 * 4], 0
        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m2dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2m2dnrv16_top


p2m2dnrv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_v16
        ALIGN 4
_gl_x86_transform_points2_3d_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p2m3dv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p2m3dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2m3dv16_top


p2m3dv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_3d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points2_3d_no_rot_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2m3dnrv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2m3dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p2m3dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2m3dnrv16_top


p2m3dnrv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points2_perspective_v16
        ALIGN 4
_gl_x86_transform_points2_perspective_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p2mpv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        mov EBX, dword ptr [ EDX + 14 * 4]
        ALIGN 4
p2mpv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], 0

p2mpv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p2mpv16_top


p2mpv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_general_v16
        ALIGN 4
_gl_x86_transform_points3_general_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3mgv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3mgv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        fadd dword ptr [ EDX + 15 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p3mgv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3mgv16_top


p3mgv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_identity_v16
        ALIGN 4
_gl_x86_transform_points3_identity_v16:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p3miv16_finish

        mov EAX, dword ptr [ ESP + 16+16]


        cmp EDI, ESI
        je   p3miv16_finish

        ALIGN 4
p3miv16_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EBP, dword ptr [ ESI + 1 * 4]
        mov EDX, dword ptr [ ESI + 2 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EBP
        mov dword ptr [ EDI + 2 * 4], EDX
        mov dword ptr [ EDI + 3 * 4], 1065353216
p3miv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3miv16_top


p3miv16_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_v16
        ALIGN 4
_gl_x86_transform_points3_2d_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]

        mov EBX, dword ptr [ ESI + 2 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3m2dv16_top


p3m2dv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_2d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points3_2d_no_rot_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3m2dnrv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3m2dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]

        fxch ST(2)
        faddp ST(2), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m2dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3m2dnrv16_top


p3m2dnrv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_v16
        ALIGN 4
_gl_x86_transform_points3_3d_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fxch ST(1)
        fadd dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        fadd dword ptr [ EDX + 14 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3m3dv16_top


p3m3dv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_3d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points3_3d_no_rot_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p3m3dnrv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p3m3dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        fadd dword ptr [ EDX + 12 * 4]
        fld dword ptr [ EDX + 13 * 4]
        fxch ST(2)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(3)
        faddp ST(3), ST(0)

        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]

        mov dword ptr [ EDI + 3 * 4], 1065353216
p3m3dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3m3dnrv16_top


p3m3dnrv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points3_perspective_v16
        ALIGN 4
_gl_x86_transform_points3_perspective_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p3mpv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p3mpv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)
        fld dword ptr [ EDX + 14 * 4]
        fxch ST(1)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p3mpv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p3mpv16_top


p3mpv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_general_v16
        ALIGN 4
_gl_x86_transform_points4_general_v16:

        push ESI
        push EDI


        mov ESI, dword ptr [ ESP + 8+12]
        mov EDI, dword ptr [ ESP + 8+4]

        mov EDX, dword ptr [ ESP + 8+8]
        mov ECX, dword ptr [ ESP + 8+20]

        test ECX, ECX
        jz   p4mgv16_finish

        mov EAX, dword ptr [ ESP + 8+16]


        ALIGN 4
p4mgv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 3 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 7 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 11 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 15 * 4]

        fxch ST(3)
        faddp ST(7), ST(0)
        fxch ST(1)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fxch ST(3)
        fstp dword ptr [ EDI + 0 * 4]
        fxch ST(1)
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

p4mgv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4mgv16_top


p4mgv16_finish:
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_identity_v16
        ALIGN 4
_gl_x86_transform_points4_identity_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4miv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        cmp EDI, ESI
        je   p4miv16_finish

        ALIGN 4
p4miv16_top:

        mov EBX, dword ptr [ ESI + 0 * 4]
        mov EDX, dword ptr [ ESI + 1 * 4]

        mov dword ptr [ EDI + 0 * 4], EBX
        mov dword ptr [ EDI + 1 * 4], EDX

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EDX, dword ptr [ ESI + 3 * 4]

        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EDX
p4miv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4miv16_top


p4miv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_v16
        ALIGN 4
_gl_x86_transform_points4_2d_v16:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dv16_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4m2dv16_top


p4m2dv16_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_2d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points4_2d_no_rot_v16:

        push ESI
        push EDI

        push EBX
        push EBP

        mov ESI, dword ptr [ ESP + 16+12]
        mov EDI, dword ptr [ ESP + 16+4]

        mov EDX, dword ptr [ ESP + 16+8]
        mov ECX, dword ptr [ ESP + 16+20]

        test ECX, ECX
        jz   p4m2dnrv16_finish

        mov EAX, dword ptr [ ESP + 16+16]


        ALIGN 4
p4m2dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]

        fxch ST(1)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        mov EBP, dword ptr [ ESI + 3 * 4]

        fxch ST(1)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        mov dword ptr [ EDI + 2 * 4], EBX
        mov dword ptr [ EDI + 3 * 4], EBP

p4m2dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4m2dnrv16_top


p4m2dnrv16_finish:
        pop EBP
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_v16
        ALIGN 4
_gl_x86_transform_points4_3d_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 1 * 4]
        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 2 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 4 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]
        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 6 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4m3dv16_top


p4m3dv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_3d_no_rot_v16
        ALIGN 4
_gl_x86_transform_points4_3d_no_rot_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4m3dnrv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4m3dnrv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 12 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 13 * 4]
        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        fxch ST(2)
        faddp ST(5), ST(0)
        faddp ST(3), ST(0)
        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 3 * 4]

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4m3dnrv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4m3dnrv16_top


p4m3dnrv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret


        PUBLIC _gl_x86_transform_points4_perspective_v16
        ALIGN 4
_gl_x86_transform_points4_perspective_v16:

        push ESI
        push EDI

        push EBX

        mov ESI, dword ptr [ ESP + 12+12]
        mov EDI, dword ptr [ ESP + 12+4]

        mov EDX, dword ptr [ ESP + 12+8]
        mov ECX, dword ptr [ ESP + 12+20]

        test ECX, ECX
        jz   p4mpv16_finish

        mov EAX, dword ptr [ ESP + 12+16]


        ALIGN 4
p4mpv16_top:


        fld dword ptr [ ESI + 0 * 4]
        fmul dword ptr [ EDX + 0 * 4]

        fld dword ptr [ ESI + 1 * 4]
        fmul dword ptr [ EDX + 5 * 4]

        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 8 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 9 * 4]
        fld dword ptr [ ESI + 2 * 4]
        fmul dword ptr [ EDX + 10 * 4]

        fxch ST(2)
        faddp ST(4), ST(0)
        faddp ST(2), ST(0)

        fld dword ptr [ ESI + 3 * 4]
        fmul dword ptr [ EDX + 14 * 4]

        faddp ST(1), ST(0)

        mov EBX, dword ptr [ ESI + 2 * 4]
        xor EBX, -2147483648

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        mov dword ptr [ EDI + 3 * 4], EBX

p4mpv16_skip:
        add EDI, 64
        add ESI, EAX
        dec ECX
        jnz   p4mpv16_top


p4mpv16_finish:
        pop EBX
        pop EDI
        pop ESI
        ret

CODE32 ENDS

DATA32 SEGMENT

clip_table:
        db 0, 1, 0, 2, 4, 5, 4, 6
        db 0, 1, 0, 2, 8, 9, 8, 10
        db 32, 33, 32, 34, 36, 37, 36, 38
        db 32, 33, 32, 34, 40, 41, 40, 42
        db 0, 1, 0, 2, 4, 5, 4, 6
        db 0, 1, 0, 2, 8, 9, 8, 10
        db 16, 17, 16, 18, 20, 21, 20, 22
        db 16, 17, 16, 18, 24, 25, 24, 26
        db 63, 61, 63, 62, 55, 53, 55, 54
        db 63, 61, 63, 62, 59, 57, 59, 58
        db 47, 45, 47, 46, 39, 37, 39, 38
        db 47, 45, 47, 46, 43, 41, 43, 42
        db 63, 61, 63, 62, 55, 53, 55, 54
        db 63, 61, 63, 62, 59, 57, 59, 58
        db 31, 29, 31, 30, 23, 21, 23, 22
        db 31, 29, 31, 30, 27, 25, 27, 26
DATA32 ENDS

CODE32 SEGMENT PARA USE32 PUBLIC 'CODE'

        PUBLIC _gl_x86_cliptest_points4
        ALIGN 4

_gl_x86_cliptest_points4:
        push ESI
        push EDI
        push EBP
        push EBX


        mov ESI, dword ptr [ ESP + 16+4]
        mov EDI, dword ptr [ ESP + 16+8]

        mov EDX, dword ptr [ ESP + 16+12]
        mov EBX, dword ptr [ ESP + 16+16]

        mov EBP, dword ptr [ ESP + 16+20]
        mov EAX, dword ptr [ ESI + 12]

        mov ECX, dword ptr [ ESI + 8]
        mov ESI, dword ptr [ ESI + 4]

        or dword ptr [ EDI + 20], 15
        mov dword ptr [ ESP + 16+4], EAX

        mov dword ptr [ EDI + 16], 3
        mov dword ptr [ EDI + 8], ECX

        mov EDI, dword ptr [ EDI + 4]
        add ECX, EDX

        mov dword ptr [ ESP + 16+12], ECX
        cmp EDX, ECX

        mov AL, byte ptr [EBX]
        mov AH, byte ptr [EBP]

        jz   ctp4_finish

        ALIGN 4
ctp4_top:
        fld1
        fdiv dword ptr [ ESI + 3 * 4]

        mov EBP, dword ptr [ ESI + 3 * 4]
        mov EBX, dword ptr [ ESI + 2 * 4]

        xor ECX, ECX
        add EBP, EBP

        adc ECX, ECX
        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX
        mov EBX, dword ptr [ ESI + 1 * 4]

        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX
        mov EBX, dword ptr [ ESI + 0 * 4]

        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX

        mov CL, byte ptr [ECX + clip_table]

        or AL, CL
        and AH, CL

        test CL, CL
        mov byte ptr [EDX], CL

        jz   ctp4_proj

        fstp ST(0)
        jmp ctp4_next

ctp4_proj:
        fld dword ptr [ ESI + 0 * 4]
        fmul ST(0), ST(1)

        fld dword ptr [ ESI + 1 * 4]
        fmul ST(0), ST(2)

        fld dword ptr [ ESI + 2 * 4]
        fmul ST(0), ST(3)

        fxch ST(2)
        fstp dword ptr [ EDI + 0 * 4]
        fstp dword ptr [ EDI + 1 * 4]
        fstp dword ptr [ EDI + 2 * 4]
        fstp dword ptr [ EDI + 3 * 4]

ctp4_next:
        inc EDX
        add EDI, 16

        add ESI, dword ptr [ ESP + 16+4]
        cmp dword ptr [ ESP + 16+12], EDX

        jnz   ctp4_top

        mov ECX, dword ptr [ ESP + 16+16]
        mov EDX, dword ptr [ ESP + 16+20]

        mov byte ptr [ECX], AL
        mov byte ptr [EDX], AH

ctp4_finish:
        mov EAX, dword ptr [ ESP + 16+8]
        pop EBX
        pop EBP
        pop EDI
        pop ESI

        ret

        PUBLIC _gl_v16_x86_cliptest_points4
        ALIGN 4

_gl_v16_x86_cliptest_points4:
        push ESI
        push EDI
        push EBP
        push EBX


        mov ESI, dword ptr [ ESP + 16+4]
        mov EDX, dword ptr [ ESP + 16+8]
        mov EBX, dword ptr [ ESP + 16+12]
        mov EBP, dword ptr [ ESP + 16+16]
        mov EDI, dword ptr [ ESP + 16+20]

        cmp ESI, EDX

        mov AL, byte ptr [EBX]
        mov AH, byte ptr [EBP]

        jz   v16_ctp4_finish

        ALIGN 4
v16_ctp4_top:

        mov EBP, dword ptr [ ESI + 3 * 4]
        mov EBX, dword ptr [ ESI + 2 * 4]

        xor ECX, ECX
        add EBP, EBP

        adc ECX, ECX
        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX
        mov EBX, dword ptr [ ESI + 1 * 4]

        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX
        mov EBX, dword ptr [ ESI + 0 * 4]

        add EBX, EBX

        adc ECX, ECX
        cmp EBP, EBX

        adc ECX, ECX

        mov CL, byte ptr [ECX + clip_table]

        or AL, CL
        and AH, CL

        mov byte ptr [EDI], CL
        inc EDI


        add ESI, 64

        cmp ESI, EDX
        jnz   v16_ctp4_top

        mov ECX, dword ptr [ ESP + 16+12]
        mov EDX, dword ptr [ ESP + 16+16]

        mov byte ptr [ECX], AL
        mov byte ptr [EDX], AH

v16_ctp4_finish:

        pop EBX
        pop EBP
        pop EDI
        pop ESI

        ret

CODE32 ENDS

end
