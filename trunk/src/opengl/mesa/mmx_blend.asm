; $Id: mmx_blend.asm,v 1.1 2000-03-01 18:49:33 jeroen Exp $
;
; MMX versions of the blending functions
;

         .586p
         .MMX

CODE32 SEGMENT PARA USE32 PUBLIC 'CODE'
CODE32 ENDS
DATA32 SEGMENT PARA USE32 PUBLIC 'DATA'
ASSUME DS:FLAT, SS:FLAT, ES:FLAT
DATA32 ENDS

CODE32 SEGMENT
ALIGN 16
PUBLIC _gl_mmx_blend_transparency

_gl_mmx_blend_transparency:
    push EBP
    mov EBP, ESP
    sub ESP, 52
    push EBX
    mov dword ptr [EBP + -8], 16711680
    mov dword ptr [EBP + -4], 16711680
    mov dword ptr [EBP + -16], 0
    mov dword ptr [EBP + -12], -1
    mov dword ptr [EBP + -24], -1
    mov dword ptr [EBP + -20], 0
    mov EAX, dword ptr [EBP + 24]
    add EAX, 4
    mov EDX, EAX
    and EDX, dword ptr [EBP + 20]
    mov EAX, EDX
    and EAX, 4
    cmp EAX, 8
    jne   GMBT_2
    mov EAX, dword ptr [EBP + 20]
    add EAX, 3
    xor EDX, EDX
    mov DL, byte ptr [EAX]
    mov dword ptr [EBP + -32], EDX
    mov EAX, 255
    mov EBX, EAX
    sub EBX, dword ptr [EBP + -32]
    mov dword ptr [EBP + -36], EBX
    mov EAX, dword ptr [EBP + 20]
    xor EDX, EDX
    mov DL, byte ptr [EAX]
    mov EAX, EDX
    imul EAX, dword ptr [EBP + -32]
    mov EDX, dword ptr [EBP + 24]
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EDX, ECX
    imul EDX, dword ptr [EBP + -36]
    add EAX, EDX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -40], EBX
    mov EAX, dword ptr [EBP + 20]
    inc EAX
    xor EDX, EDX
    mov DL, byte ptr [EAX]
    mov EAX, EDX
    imul EAX, dword ptr [EBP + -32]
    mov EDX, dword ptr [EBP + 24]
    inc EDX
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EDX, ECX
    imul EDX, dword ptr [EBP + -36]
    add EAX, EDX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -44], EBX
    mov EAX, dword ptr [EBP + 20]
    add EAX, 2
    xor EDX, EDX
    mov DL, byte ptr [EAX]
    mov EAX, EDX
    imul EAX, dword ptr [EBP + -32]
    mov EDX, dword ptr [EBP + 24]
    add EDX, 2
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EDX, ECX
    imul EDX, dword ptr [EBP + -36]
    add EAX, EDX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -48], EBX
    mov EAX, dword ptr [EBP + 20]
    add EAX, 3
    xor EDX, EDX
    mov DL, byte ptr [EAX]
    mov EAX, EDX
    imul EAX, dword ptr [EBP + -32]
    mov EDX, dword ptr [EBP + 24]
    add EDX, 3
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EDX, ECX
    imul EDX, dword ptr [EBP + -36]
    add EAX, EDX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -52], EBX
    mov EAX, dword ptr [EBP + 20]
    mov DL, byte ptr [EBP + -40]
    mov byte ptr [EAX], DL
    mov EAX, dword ptr [EBP + 20]
    inc EAX
    mov DL, byte ptr [EBP + -44]
    mov byte ptr [EAX], DL
    mov EAX, dword ptr [EBP + 20]
    add EAX, 2
    mov DL, byte ptr [EBP + -48]
    mov byte ptr [EAX], DL
    mov EAX, dword ptr [EBP + 20]
    add EAX, 3
    mov DL, byte ptr [EBP + -52]
    mov byte ptr [EAX], DL
    inc dword ptr [EBP + 16]
    add dword ptr [EBP + 20], 4
    add dword ptr [EBP + 24], 4
    dec dword ptr [EBP + 12]
GMBT_2:

    cmp dword ptr [EBP + 12], 0
    je   GMBT_3
    mov dword ptr [EBP + -28], 0
ALIGN 4
GMBT_4:

    mov EDX, dword ptr [EBP + 12]
    mov EAX, EDX
    shr EAX, 1
    cmp dword ptr [EBP + -28], EAX
    jb   GMBT_7
    jmp GMBT_5
ALIGN 16
GMBT_7:

    mov EAX, dword ptr [EBP + -28]
    lea EDX, [EAX * 2 + 0]
    mov EAX, dword ptr [EBP + 16]
    cmp byte ptr [EAX + EDX], 0
    je   GMBT_6
    mov EAX, dword ptr [EBP + -28]
    mov EDX, EAX
    lea ECX, [EDX * 8 + 0]
    mov EAX, ECX
    add EAX, dword ptr [EBP + 20]
    mov EDX, dword ptr [EBP + -28]
    mov ECX, EDX
    lea EDX, [ECX * 8 + 0]
    mov ECX, EDX
    add ECX, dword ptr [EBP + 24]

    movq mm4, [EAX]
    pxor mm5, mm5
    movq mm1, mm4
    movq mm7, [ECX]
    punpcklbw mm1, mm5
    movq mm6, mm7
    movq mm0, mm1
    punpcklbw mm6, mm5
    movq mm2, mm1
    psrlq mm0, 48
    punpckhbw mm4, mm5
    packssdw mm0, mm0
    movq mm3, mm0
    punpckhbw mm7, mm5
    psllq mm3, 16
    por mm0, [EBP + -8]
    punpcklwd mm1, mm6
    psubw mm0, mm3
    punpckhwd mm2, mm6
    movq mm3, mm4
    psrlq mm3, 48
    packssdw mm3, mm3
    movq mm6, mm3
    por mm3, [EBP + -8]
    psllq mm6, 16
    psubw mm3, mm6
    movq mm5, mm4
    punpcklwd mm4, mm7
    punpckhwd mm5, mm7
    pmaddwd mm1, mm0
    pmaddwd mm4, mm3
    pmaddwd mm2, mm0
    pmaddwd mm5, mm3
    psrld mm1, 8
    psrld mm2, 8
    psrld mm4, 8
    packssdw mm1, mm2
    psrld mm5, 8
    packuswb mm1, mm1
    packssdw mm4, mm5
    pand mm1, [EBP + -24]
    packuswb mm4, mm4
    pand mm4, [EBP + -16]
    por mm4, mm1
    movq [EAX], mm4


GMBT_8:

GMBT_6:

    inc dword ptr [EBP + -28]
    jmp GMBT_4
ALIGN 16
GMBT_5:


    emms

GMBT_3:

    mov EAX, dword ptr [EBP + 12]
    and EAX, 1
    test EAX, EAX
    je   GMBT_9
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -1]
    xor EAX, EAX
    mov AL, byte ptr [EDX]
    mov dword ptr [EBP + -52], EAX
    mov EAX, 255
    mov EBX, EAX
    sub EBX, dword ptr [EBP + -52]
    mov dword ptr [EBP + -48], EBX
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -4]
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EAX, ECX
    imul EAX, dword ptr [EBP + -52]
    mov EDX, dword ptr [EBP + 12]
    lea ECX, [EDX * 4 + 0]
    mov EDX, ECX
    add EDX, dword ptr [EBP + 24]
    lea ECX, [EDX + -4]
    xor EDX, EDX
    mov DL, byte ptr [ECX]
    mov ECX, EDX
    imul ECX, dword ptr [EBP + -48]
    add EAX, ECX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -44], EBX
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -3]
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EAX, ECX
    imul EAX, dword ptr [EBP + -52]
    mov EDX, dword ptr [EBP + 12]
    lea ECX, [EDX * 4 + 0]
    mov EDX, ECX
    add EDX, dword ptr [EBP + 24]
    lea ECX, [EDX + -3]
    xor EDX, EDX
    mov DL, byte ptr [ECX]
    mov ECX, EDX
    imul ECX, dword ptr [EBP + -48]
    add EAX, ECX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -40], EBX
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -2]
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EAX, ECX
    imul EAX, dword ptr [EBP + -52]
    mov EDX, dword ptr [EBP + 12]
    lea ECX, [EDX * 4 + 0]
    mov EDX, ECX
    add EDX, dword ptr [EBP + 24]
    lea ECX, [EDX + -2]
    xor EDX, EDX
    mov DL, byte ptr [ECX]
    mov ECX, EDX
    imul ECX, dword ptr [EBP + -48]
    add EAX, ECX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -36], EBX
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -1]
    xor ECX, ECX
    mov CL, byte ptr [EDX]
    mov EAX, ECX
    imul EAX, dword ptr [EBP + -52]
    mov EDX, dword ptr [EBP + 12]
    lea ECX, [EDX * 4 + 0]
    mov EDX, ECX
    add EDX, dword ptr [EBP + 24]
    lea ECX, [EDX + -1]
    xor EDX, EDX
    mov DL, byte ptr [ECX]
    mov ECX, EDX
    imul ECX, dword ptr [EBP + -48]
    add EAX, ECX
    mov EBX, EAX
    sar EBX, 8
    mov dword ptr [EBP + -32], EBX
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -4]
    mov AL, byte ptr [EBP + -44]
    mov byte ptr [EDX], AL
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -3]
    mov AL, byte ptr [EBP + -40]
    mov byte ptr [EDX], AL
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -2]
    mov AL, byte ptr [EBP + -36]
    mov byte ptr [EDX], AL
    mov EAX, dword ptr [EBP + 12]
    lea EDX, [EAX * 4 + 0]
    mov EAX, EDX
    add EAX, dword ptr [EBP + 20]
    lea EDX, [EAX + -1]
    mov AL, byte ptr [EBP + -32]
    mov byte ptr [EDX], AL
GMBT_9:

GMBT_1:

    mov EBX, dword ptr [EBP + -56]
    mov ESP, EBP
    pop EBP
    ret

CODE32 ENDS

end
