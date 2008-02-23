/* $Id: krnlPrologs.c,v 1.3 2002-12-19 01:49:09 bird Exp $
 *
 * krnlPrologs - Kernel Prolog Recognizer.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlPrologs.c,v 1.3 2002-12-19 01:49:09 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#if  0
    #define kprintf2(a) kprintf
#else
    #define kprintf2(a) (void)0
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_KKL_LOG
#include "kKrnlLib.h"



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
INLINE int  ModR_M_32bit(char bModRM);
INLINE int  ModR_M_16bit(char bModRM);
int         interpretFunctionProlog32(char *pach);
int         interpretFunctionProlog16(char *pach);



/**
 * Functions which cacluates the instructionsize given a ModR/M byte.
 * @returns   Number of bytes to add to cb and pach.
 * @param     bModRM  ModR/M byte.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int ModR_M_32bit(char bModRM)
{
    KLOGENTRY1("int","char bModRM", bModRM);
    if ((bModRM & 0xc0) == 0x80  /* ex. mov ax,[ebp+11145543h] */
        || ((bModRM & 0xc0) == 0 && (bModRM & 0x07) == 5)) /* ex. mov ebp,[0ff231234h] */
    {   /* 32-bit displacement */
        KLOGEXIT(5 + ((bModRM & 0x7) == 0x4));
        return 5 + ((bModRM & 0x7) == 0x4); // + SIB
    }
    else if ((bModRM & 0xc0) == 0x40) /* ex. mov ecx,[esi]+4fh */
    {   /* 8-bit displacement */
        KLOGEXIT(2 + ((bModRM & 0x7) == 0x4));
        return 2 + ((bModRM & 0x7) == 0x4); // + SIB
    }
    /* no displacement (only /r byte) */
    KLOGEXIT(1);
    return 1;
}


/**
 * Functions which cacluates the instructionsize given a ModR/M byte.
 * @returns   Number of bytes to add to cb and pach.
 * @param     bModRM  ModR/M byte.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int ModR_M_16bit(char bModRM)
{
    KLOGENTRY1("int","char bModRM", bModRM);
    if ((bModRM & 0xc0) == 0x80  /* ex. mov ax,[ebp+11145543h] */
        || ((bModRM & 0xc0) == 0 && (bModRM & 0x07) == 5)) /* ex. mov ebp,[0ff231234h] */
    {   /* 16-bit displacement */
        KLOGEXIT(4);
        return 4;
    }
    else if ((bModRM & 0xc0) == 0x40) /* ex. mov ecx,[esi]+4fh */
    {   /* 8-bit displacement */
        KLOGEXIT(2);
        return 2;
    }
    /* no displacement (only /r byte) */
    KLOGEXIT(1);
    return 1;
}


/**
 * 32-bit! Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 */
int krnlInterpretProlog32(char *pach)
{
    KLOGENTRY1("int","char * pach", pach);
    int cb = -3;
    kprintf2(("0x%08x:\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n",
              pach,
              pach[0], pach[1], pach[2], pach[3], pach[4], pach[5], pach[6], pach[7],
              pach[8], pach[9], pach[10],pach[11],pach[12],pach[13],pach[14],pach[15]));

    /*
     * check for the well known prolog (the only that is supported now)
     * which is:
     *     push ebp
     *     mov ebp,esp
     *  or
     *     push ebp
     *     mov eax, dword ptr [xxxxxxxx]
     *  or
     *     sub esp, imm8
     *     push ebx
     *     push edi
     *
     * These are allowed when not overloading:
     *     mov eax, imm32
     *     jmp short
     *  or
     *     mov eax, imm32
     *     push ebp
     *  or
     *     mov ecx, r/m32
     *  or
     *     jmp dword
     *  or
     *     sub esp, imm8
     *  or
     *     call ptr16:32
     *  or
     *     enter imm16, imm8    (2.1x)
     *  or
     *     mov eax, imm32       (2.1x)
     *     <anything>
     *  or
     *     xor r32, r/m32
     *  or
     *     mov eax, msoff32
     *  or
     *     push edi
     *     mov eax, dword ptr [xxxxxxxx]
     *  or
     *     movzx esp, sp
     *  or
     *     call rel32
     *     popf
     */
    if ((pach[0] == 0x55 && (pach[1] == 0x8b || pach[1] == 0xa1)) /* the two first prologs */
        ||
        (pach[0] == 0x83 &&  pach[3] == 0x53 && pach[4] == 0x57)  /* the third prolog */
        ||
        (pach[0] == 0xB8 && (pach[5] == 0xEB || pach[5] == 0x55)) /* the two next prologs */
        ||
        (pach[0] == 0x8B) /* the next prolog */
        ||
        (pach[0] == 0xFF) /* the next prolog */
        ||
        (pach[0] == 0x83) /* the next prolog */
        ||
        (pach[0] == 0x9a) /* the next prolog */
        ||
        (pach[0] == 0xc8)               /* the next prolog */
        ||
        (pach[0] == 0xB8) /* the next prolog */
        ||
        (pach[0] == 0x33) /* the next prolog */
        ||
        (pach[0] == 0xa1) /* the next prolog */
        ||
        (pach[0] == 0x57 &&  pach[1] == 0x8b) /* the next prolog */
        ||
        (pach[0] == 0x0f &&  pach[1] == 0xb7 && pach[2] == 0xe4) /* the next prolog */
        ||
        (pach[0] == 0xe8 &&  pach[5] == 0x9d) /* the next prolog */
        /*      push ebx
         *      push edi
         *      push imm32
         */
        || (pach[0] == 0x53 &&  pach[1] == 0x57 &&  pach[2] == 0x68)
        /*      push ebx
         *      push edi
         *      push esi
         *      mov /r
         */
        || (pach[0] == 0x53 &&  pach[1] == 0xe8)
        /*      push ebx
         *      push edi
         *      push esi
         *      mov /r
         */
        || (pach[0] == 0x53 &&  pach[1] == 0x57 &&  pach[2] == 0x56 &&  pach[3] == 0x8b)
        /*      pop  eax
         *      push imm8
         *      push eax
         *      jmp  imm32
         */
        ||  (pach[0] == 0x58 &&  pach[1] == 0x6a &&  pach[3] == 0x50 &&  pach[4] == 0xe9)
        /*      push imm32
         *      call imm32
         */
        || (pach[0] == 0x68 && pach[5] == 0xe8)
        /*      push    ebp
         *      xor     ecx, ecx
         *      mov     ebp, esp
         */
        ||  (pach[0] == 0x55 &&  pach[1] == 0x33 &&  pach[2] == 0xc9 &&  pach[3] == 0x8b && pach[4] == 0xec)
        /*      push    ebx
         *      mov     ax, word ptr [esp+8]
         */
        ||  (pach[0] == 0x53 &&  pach[1] == 0x66 &&  pach[2] == 0x8b &&  pach[3] == 0x44 && pach[4] == 0x24)
        )
    {
        KBOOL fForce = FALSE;
        int  cbWord = 4;
        cb = 0;
        while (cb < 5 || fForce)                  /* 5 is the size of a jump instruction. */
        {
            int cb2;
            if (!fForce && cbWord != 4)
                cbWord = 4;
            fForce = FALSE;
            switch (*pach)
            {
                case 0x0f:
                    if (pach[1] != 0xb7 && pach[2] != 0xe4) /* movzx esp, sp */
                    {
                        kprintf(("unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        KLOGEXIT(-11);
                        return -11;
                    }
                    pach += 2;
                    cb += 2;
                    break;


                /* simple one byte prefixes */
                case 0x2e:              /* cs segment override */
                case 0x36:              /* ss segment override */
                case 0x3e:              /* ds segment override */
                case 0x26:              /* es segment override */
                case 0x64:              /* fs segment override */
                case 0x65:              /* gs segment override */
                    fForce = TRUE;
                    break;

                case 0x66:              /* 16 bit */
                    fForce = TRUE;
                    cbWord = 2;
                    break;

                /* simple one byte instructions e*/
                case 0x50:              /* push eax */
                case 0x51:              /* push ecx */
                case 0x52:              /* push edx */
                case 0x53:              /* push ebx */
                case 0x54:              /* push esp */
                case 0x55:              /* push ebp */
                case 0x56:              /* push esi */
                case 0x57:              /* push edi */
                case 0x58:              /* pop  eax */
                case 0x59:              /* pop  ecx */
                case 0x5a:              /* pop  edx */
                case 0x5b:              /* pop  ebx */
                case 0x06:              /* push es */
                case 0x0e:              /* push cs */
                case 0x1e:              /* push ds */
                case 0x16:              /* push ss */
                    break;

                /* simple two byte instructions */
                case 0xb0:              /* mov al, imm8 */
                case 0xb1:              /* mov cl, imm8 */
                case 0xb2:              /* mov dl, imm8 */
                case 0xb3:              /* mov bl, imm8 */
                case 0xb4:              /* mov ah, imm8 */
                case 0xb5:              /* mov ch, imm8 */
                case 0xb6:              /* mov dh, imm8 */
                case 0xb7:              /* mov bh, imm8 */
                case 0x2c:              /* sub al, imm8 */
                case 0x34:              /* xor al, imm8 */
                case 0x3c:              /* cmp al, imm8 */
                case 0x6a:              /* push <byte> */
                case 0xa0:              /* mov al, moffs8 */
                case 0xa2:              /* mov moffs8, al */
                    pach++;
                    cb++;
                    break;

                /* simple five byte instructions */
                case 0xb8:              /* mov eax, imm32 */
                case 0xb9:              /* mov ecx, imm32 */
                case 0xba:              /* mov edx, imm32 */
                case 0xbb:              /* mov ebx, imm32 */
                case 0xbc:              /* mov esx, imm32 */
                case 0xbd:              /* mov ebx, imm32 */
                case 0xbe:              /* mov esi, imm32 */
                case 0xbf:              /* mov edi, imm32 */
                case 0xe9:              /* jmp rel32 */
                case 0x2d:              /* sub eax, imm32 */
                case 0x35:              /* xor eax, imm32 */
                case 0x3d:              /* cmp eax, imm32 */
                case 0x68:              /* push <dword> */
                case 0xa1:              /* mov eax, moffs16 */
                case 0xa3:              /* mov moffs16, eax */
                    pach += cbWord;
                    cb += cbWord;
                    break;

                /* fixed five byte instructions */
                case 0xe8:              /* call imm32 */
                    pach += 4;
                    cb += 4;
                    break;

                /* complex sized instructions -  "/r" */
                case 0x30:              /* xor r/m8,  r8 */
                case 0x31:              /* xor r/m32, r32 */
                case 0x32:              /* xor r8,  r/m8 */
                case 0x33:              /* xor r32, r/m32 */
                case 0x38:              /* cmp r/m8, r8 */
                case 0x39:              /* cmp r/m32, r32 */
                case 0x3a:              /* cmp r8, r/m8 */
                case 0x3b:              /* cmp r32, r/m32 */
                case 0x28:              /* sub r/m8, r8 */
                case 0x29:              /* sub r/m32, r32 */
                case 0x2a:              /* sub r8, r/m8 */
                case 0x2b:              /* sub r32, r/m32 */
                case 0x8b:              /* mov /r */
                case 0x8d:              /* lea /r */
                    cb += cb2 = ModR_M_32bit(pach[1]);
                    pach += cb2;
                    break;

                /* complex sized instruction - "/5 ib" */
                case 0x80:              /* 5: sub r/m8, imm8  7: cmp r/m8, imm8 */
                case 0x83:              /* 5: sub r/m32, imm8 7: cmp r/m32, imm8 */
                    if ((pach[1] & 0x38) == (5<<3)
                        || (pach[1] & 0x38) == (7<<3)
                        )
                    {
                        cb += cb2 = 1 + ModR_M_32bit(pach[1]); /* 1 is the size of the imm8 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("unknown instruction (-3) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        KLOGEXIT(-3);
                        return -3;
                    }
                    break;

                /* complex sized instruction - "/digit id" */
                case 0x81:              /* sub r/m32, imm32 + more instructions! */
                    if ((pach[1] & 0x38) == (5<<3)       /* sub r/m32, imm32  */
                        || (pach[1] & 0x38) == (7<<3)    /* cmp r/m32, imm32  */
                        )
                    {
                        cb += cb2 = cbWord + ModR_M_32bit(pach[1]); /* cbWord is the size of the imm32/imm16 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("unknown instruction (-2) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        KLOGEXIT(-2);
                        return -2;
                    }
                    break;

                case 0x9a:              /* call ptr16:32 */
                    cb += cb2 = 6;
                    pach += cb2;
                    break;

                case 0xc8:              /* enter imm16, imm8 */
                    cb += cb = 3;
                    pach += cb2;
                    break;

                /*
                 * jmp /digit
                 */
                case 0xff:
                    cb += cb2 = cbWord + ModR_M_32bit(pach[1]); /* cbWord is the size of the imm32/imm16 */
                    pach += cb2;
                    break;

                default:
                    kprintf(("unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                    KLOGEXIT(0);
                    return 0;
            }
            pach++;
            cb++;
        }
    }
    else
    {
        kprintf(("unknown prolog start. 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                 pach[0], pach[1], pach[2], pach[3], pach[4], pach[5]));
        cb = 0;
    }
    KLOGEXIT(cb);
    return cb;
}


/**
 * 16-bit! Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 */
int krnlInterpretProlog16(char *pach)
{
    KLOGENTRY1("int","char * pach", pach);
    int cb = -7;

    kprintf2(("0x%08x:\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n",
              pach, ,
              pach[0], pach[1], pach[2], pach[3], pach[4], pach[5], pach[6], pach[7],
              pach[8], pach[9], pach[10],pach[11],pach[12],pach[13],pach[14],pach[15]));
    /*
     * Check for the well known prolog (the only that is supported now)
     * which is:
     */
    if ((*pach == 0x6A)                     /* push 2 (don't check for the 2) */
        ||
        *pach == 0x60                       /* pushf */
        ||
        (*pach == 0x53 && pach[1] == 0x51)  /* push bx, push cx */
        ||
        (*pach == 0x8c && pach[1] == 0xd8)  /* mov ax, ds */
        ||
        (*pach == 0xb8)                     /* mov ax, imm16 */
        )
    {
        KBOOL fForce;
        int  cOpPrefix = 0;
        cb = 0;
        while (cb < 5  || fForce)       /* 5 is the size of a 16:16 far jump instruction. */
        {
            int cb2;
            fForce = FALSE;
            switch (*pach)
            {
                case 0x06:              /* push es */
                case 0x0e:              /* push cs */
                case 0x1e:              /* push ds */
                case 0x16:              /* push ss */
                    break;

                case 0x0f:              /* push gs and push fs */
                    if (pach[1] != 0xA0 && pach[1] != 0xA8)
                    {
                        kprintf(("unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        KLOGEXIT(-11);
                        return -11;
                    }
                    pach++;
                    cb++;
                    break;

                case 0x50:              /* push ax */
                case 0x51:              /* push cx */
                case 0x52:              /* push dx */
                case 0x53:              /* push bx */
                case 0x54:              /* push sp */
                case 0x55:              /* push bp */
                case 0x56:              /* push si */
                case 0x57:              /* push di */
                case 0x60:              /* pusha */
                    break;

                /* simple three byte instructions */
                case 0xb8:              /* mov eax, imm16 */
                case 0xb9:              /* mov ecx, imm16 */
                case 0xba:              /* mov edx, imm16 */
                case 0xbb:              /* mov ebx, imm16 */
                case 0xbc:              /* mov esx, imm16 */
                case 0xbd:              /* mov ebx, imm16 */
                case 0xbe:              /* mov esi, imm16 */
                case 0xbf:              /* mov edi, imm16 */
                case 0x2d:              /* sub eax, imm16 */
                case 0x35:              /* xor eax, imm16 */
                case 0x3d:              /* cmp eax, imm16 */
                case 0x68:              /* push <dword> */
                case 0xa1:              /* mov eax, moffs16 */
                case 0xa3:              /* mov moffs16, eax */
                    if (cOpPrefix > 0)  /* FIXME see 32-bit interpreter. */
                    {
                        pach += 2;
                        cb += 2;
                    }
                    pach += 2;
                    cb += 2;
                    break;

                case 0x2e:              /* cs segment override */
                case 0x36:              /* ss segment override */
                case 0x3e:              /* ds segment override */
                case 0x26:              /* es segment override */
                case 0x64:              /* fs segment override */
                case 0x65:              /* gs segment override */
                    fForce = TRUE;
                    if (cOpPrefix > 0)
                        cOpPrefix++;
                    break;

                case 0x66:
                    cOpPrefix = 2;      /* it's decremented once before it's used. */
                    fForce = TRUE;
                    break;

                case 0x6a:              /* push <byte> */
                case 0x3c:              /* mov al, imm8 */
                    pach++;
                    cb++;
                    break;

                case 0x8b:              /* mov /r */
                case 0x8c:              /* mov r/m16,Sreg  (= mov /r) */
                case 0x8e:              /* mov Sreg, r/m16 (= mov /r) */
                    if ((pach[1] & 0xc0) == 0x80  /* ex. mov ax,bp+1114h */
                        || ((pach[1] & 0xc0) == 0 && (pach[1] & 0x7) == 6)) /* ex. mov bp,0ff23h */
                    {   /* 16-bit displacement */
                        if (cOpPrefix > 0)
                        {
                            pach += 2;
                            cb += 2;
                        }
                        pach += 3;
                        cb += 3;
                    }
                    else
                        if ((pach[1] & 0xc0) == 0x40) /* ex. mov ax,[si]+4fh */
                    {   /* 8-bit displacement */
                        pach += 2;
                        cb += 2;
                    }
                    else
                    {   /* no displacement (only /r byte) */
                        pach++;
                        cb++;
                    }
                    break;

                /* complex sized instruction - "/5 ib" */
                case 0x80:              /* 5: sub r/m8, imm8  7: cmp r/m8, imm8 */
                case 0x83:              /* 5: sub r/m16, imm8 7: cmp r/m16, imm8 */
                    if ((pach[1] & 0x38) == (5<<3)
                        || (pach[1] & 0x38) == (7<<3)
                        )
                    {
                        cb += cb2 = 1 + ModR_M_16bit(pach[1]); /* 1 is the size of the imm8 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("unknown instruction (-3) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        KLOGEXIT(-3);
                        return -3;
                    }
                    break;

                case 0x9a:              /* call ptr16:16 */
                    cb += cb2 = 4;
                    pach += cb2;
                    break;

                default:
                    kprintf(("unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                    KLOGEXIT(0);
                    return 0;
            }
            pach++;
            cb++;
            if (cOpPrefix > 0)
                cOpPrefix--;
        }
    }
    else
        kprintf(("unknown prolog 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));

    KLOGEXIT(cb);
    return cb;
}

