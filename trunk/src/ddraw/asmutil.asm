; asmutil.asm Color key bit blitting for DirectDraw
;
; Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
; *
; * Project Odin Software License can be found in LICENSE.TXT
; *
;

.386p
                NAME    asmutil

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:FLAT ,DS:FLAT,SS:FLAT

        PUBLIC  _BlitColorKey8

;           endpos = destbuf + blitlinesize;
;           while(destbuf < endpos) {
;               if(*srcbuf == colorkey) {
;                   destbuf++;
;               }
;               else    *destbuf++ = *srcbuf;
;               srcbuf++;
;           }
;           destbuf += (destscanlinesize-blitlinesize);
;           srcbuf  += (srcscanlinesize-blitlinesize);
;void BlitColorKey8(char *dest, char *src, ULONG key, ULONG linesize)
_BlitColorKey8  PROC NEAR
    push    ebp
    mov ebp, esp
    push    edi
    push    esi
    push    eax
    push    ebx
    push    ecx
    push    edx

    mov edi, dword ptr [ebp+8]      ;dest
    mov esi, dword ptr [ebp+12]     ;src
    mov ecx, dword ptr [ebp+20]     ;linesize
    mov edx, dword ptr [ebp+16]     ;colorkey

    and ecx, 3
    push    ecx             ;do the remaining bytes afterwards
    mov ecx, dword ptr [ebp+20]     ;linesize
    shr ecx, 2              ;linesize in dwords

   ALIGN 16

blitloop:
    mov ebx, dword ptr [esi]
    mov eax, dword ptr [edi]
    cmp bl, dl
    je  skipbyte1
    mov al, bl

skipbyte1:
    cmp bh, dl
    je  skipbyte2
    mov ah, bh
skipbyte2:
    ror eax, 16
    ror ebx, 16
    cmp bl, dl
    je  skipbyte3
    mov al, bl
skipbyte3:
    cmp bh, dl
    je  skipbyte4
    mov ah, bh
skipbyte4:
    ror eax, 16
    dec ecx
    mov dword ptr [edi], eax
    add esi, 4
    add edi, 4

    cmp ecx, 0
    jne blitloop

    pop ecx
    cmp ecx, 0
    je  endofblit

blitperbyte:
    mov al, byte ptr [esi]
    dec ecx
    cmp al, dl
    je  skipsinglebyte
    mov byte ptr [edi], al
skipsinglebyte:
    inc esi
    inc edi
    cmp ecx, 0
    jne blitperbyte

endofblit:
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop esi
    pop edi
    pop ebp
    ret
_BlitColorKey8  ENDP

CODE32          ENDS

                END
