; $Id: asmutil.asm,v 1.8 2001-03-18 21:44:44 mike Exp $

;
; asmutil.asm Color key bit blitting for DirectDraw
;
; Copyright 1998 Sander van Leeuwen
;           1999 Markus Montkowski
;
; Project Odin Software License can be found in LICENSE.TXT
;

                NAME    asmutil
.586p
.MMX

CODE32  SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32  ENDS
DATA32  SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32  ENDS
CONST32  SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST32  ENDS
BSS32  SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS32  ENDS
DGROUP  GROUP CONST32, BSS32, DATA32
  ASSUME  CS:FLAT, DS:FLAT, SS:FLAT, ES:FLAT
  DATA32  SEGMENT
  DATA32  ENDS
  BSS32  SEGMENT
  BSS32  ENDS
  CONST32  SEGMENT
  CONST32  ENDS


CODE32  SEGMENT

    PUBLIC  _BlitColorKey8

;      endpos = destbuf + blitlinesize;
;      while(destbuf < endpos) {
;        if(*srcbuf == colorkey) {
;          destbuf++;
;        }
;        else  *destbuf++ = *srcbuf;
;        srcbuf++;
;      }
;      destbuf += (destscanlinesize-blitlinesize);
;      srcbuf  += (srcscanlinesize-blitlinesize);
;void BlitColorKey8(char *dest, char *src, ULONG key, ULONG linesize)
_BlitColorKey8   PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx
  push  edx

  mov  edi, dword ptr [ebp+8]     ;dest
  mov  esi, dword ptr [ebp+12]    ;src
  mov  ecx, dword ptr [ebp+20]    ;linesize
  mov  edx, dword ptr [ebp+16]    ;colorkey

  and  ecx, 3
  mov  dh , dl
  push ecx        ;do the remaining bytes afterwards
  mov  eax, edx
  shl  edx, 16
  and  eax, 0000FFFFh
  mov  ecx, dword ptr [ebp+20]    ;linesize
  or   edx, eax      ; edx now contains the colorkey in each byte
  shr  ecx, 2        ;linesize in dwords
  jz  blitremain     ; less then 4 bytes
  jmp blitStart
blitloop:
  add  esi, 4
  add  edi, 4
blitStart:
  mov  ebx, dword ptr [esi]
  mov  eax, dword ptr [edi]
  cmp  ebx, edx   ; All 4 bytes transparent?
  jz TTTT
  cmp   bx, dx    ; lower 2 bytes transparent?
  jz XXTT
  cmp   bl, dl    ; lower byte transparent?
  jz XXOT
  mov   al, bl
  cmp   bh, dh    ; upper Byte transparent then skip copy
  jz XXTT
XXOT:
  mov   ah, bh
XXTT:   ; handle upper 2 pixel
  ror  eax, 16
  ror  ebx, 16
  cmp   bx, dx
  jz   skipbyte4
  cmp   bl, dl
  je  skipbyte3
  mov   al, bl
skipbyte3:
  cmp  bh, dh
  je  skipbyte4
  mov  ah, bh
skipbyte4:
  ror  eax, 16
  mov  dword ptr [edi], eax
TTTT:
  dec  ecx
  jnz  blitloop
blitremain:
  pop  ecx
  cmp  ecx, 2
  ja   blit3
  jz   blit2
  test ecx,ecx
  jz   endofblit
  mov  eax, dword ptr [esi]
  mov  ebx, dword ptr [edi]
  cmp  al,dl
  jz   endofblit
  mov  bl,al
  mov  dword ptr[edi],ebx
  jmp endofblit

blit3:
  mov  eax, dword ptr [esi]
  mov  ebx, dword ptr [edi]
  cmp  ax, dx
  jz   TTX
  cmp  ah, dh
  jz   TXX
  mov  bh, ah
TXX:
  cmp  al, dl
  jz   TTX
  mov  bl, al
TTX:
  ror  eax, 16
  ror  ebx, 16
  cmp  al, dl
  jz   Cpyback
  mov  bl, al
Cpyback:
  ror  ebx, 16
  mov  dword ptr [edi], ebx
  jmp  endofblit

blit2:
  mov  eax, dword ptr [esi]
  mov  ebx, dword ptr [edi]
  cmp  ax, dx  ; both bytes transparent ?
  jz   endofblit
  cmp  ah, dh
  jz   TX
  mov  bh, ah
TX:
  cmp  al, dl
  jz   OT
  mov  bl, al
OT:
  mov dword ptr[edi], ebx

endofblit:
  pop  edx
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret
_BlitColorKey8  ENDP

    PUBLIC  _BlitColorKey16

;      endpos = destbuf + blitlinesize;
;      while(destbuf < endpos) {
;        if(*srcbuf == colorkey) {
;          destbuf++;
;        }
;        else  *destbuf++ = *srcbuf;
;        srcbuf++;
;      }
;      destbuf += (destscanlinesize-blitlinesize);
;      srcbuf  += (srcscanlinesize-blitlinesize);
;void BlitColorKey16(char *dest, char *src, ULONG key, ULONG linesize)
_BlitColorKey16   PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx
  push  edx

  mov  edi, dword ptr [ebp+8]     ;dest
  mov  esi, dword ptr [ebp+12]    ;src
  mov  ecx, dword ptr [ebp+20]    ;linesize
  mov  edx, dword ptr [ebp+16]    ;colorkey

  mov  eax, edx
  shl  edx, 16;
  and  eax, 0000FFFFh
  or   edx,eax       ; create dwColorKey
  shr  ecx, 1        ; linesize in dwords
  jz   OnePixel      ; FIXME: BUG if ecx was really 1!

blitloop16:
  mov  eax, dword ptr [esi]
  mov  ebx, dword ptr [edi]
  add  esi, 4
  cmp  eax, edx  ; are both pixel transparent?
  je   LoopUp    ; Yes, then Jump to loopend
  cmp  ax, dx    ; Is lower pixel transparent
  je   DrawOT    ; Yes So We got OT (OPAQUE/Transparent
  mov  bx, ax    ; No so copy the lower pixel
DrawOT:
  ror  eax, 16   ;
  cmp  ax, dx    ; Is higher pixel transparent
  je   CopyBack  ;
  mov  bx, ax
CopyBack:
  ror ebx,16
  mov dword ptr[edi], ebx ; copy back the result in ebx
LoopUp:
  mov ebx, dword ptr [ebp+20] ; V load this this in case we are done
  add edi, 4                  ; U
OnePixel:
  dec  ecx
  jnz  blitloop16
  test ebx, 1                 ; Do we have an odd linesize
  jz   endofblit16
  mov  eax, dword ptr [esi]
  mov  ebx, dword ptr [edi]
  cmp  ax, dx
  je   endofblit16            ; last pixel is transparent
  mov  bx,ax                  ; No so copy the lower pixel
  mov  dword ptr [edi], ebx   ; copy back the result in ebx

endofblit16:
  pop  edx
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret
_BlitColorKey16  ENDP


    PUBLIC  _BlitColorKey8MMX
; Now the same as BlitColorKey8 now with MMX
;void BlitColorKey8MMX(char *dest, char *src, ULONG key, ULONG linesize)

_BlitColorKey8MMX PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx
  push  edx

  mov   edx,  [ebp+16]    ;colorkey (in dl)
  mov   edi,  [ebp+8]     ;dest
  mov   esi,  [ebp+12]    ;src
  mov   ecx, dword ptr [ebp+20]    ;linesize
  mov   dh,dl
  mov   eax,edx
  shl   edx,16
  mov   dx,ax
  movd  mm4,edx
  movd  mm5,edx
  psllq mm4,32
  por   mm4,mm5
  shr   ecx,3
  jz BltRemain8

bltLoopMMX8:
  movq mm0, [esi]  ; get source qword
  movq mm1, [edi]  ; get dest   qword
  movq mm2, mm0    ; copy source
  pcmpeqb mm0, mm4 ; create mask
  pand  mm1, mm0   ; mask dest
  pandn mm0, mm2   ; NOT mask AND source
  por   mm1, mm0   ; or them
  movq qword ptr [edi], mm1 ; write back result
  add esi, 8
  add edi, 8
  dec ecx
  jnz bltLoopMMX8
BltRemain8:
  mov  eax, dword ptr [ebp+20];
  and  eax, 7
  jmp  ds:JmpTable[eax*4]

align 4

JmpTable:
  dd offset cs:bltEndMMX8
  dd offset cs:blt1MMX8
  dd offset cs:blt2MMX8
  dd offset cs:blt3MMX8
  dd offset cs:blt4MMX8
  dd offset cs:blt5MMX8
  dd offset cs:blt6MMX8
  dd offset cs:blt7MMX8
align 2
;
; Maybe it would be faster for 7-5 to load a qword into mm0/mm1
; but we might cross a page and so I guess this is saver
;
blt7MMX8:
  movd  mm0, dword ptr[esi]
  mov   ax, word ptr[esi+4]
  mov   bx, word ptr[edi+4]
  movd  mm1, dword ptr[edi]
  psllq mm0, 32
  shl   eax, 8
  shl   ebx, 8
  mov   al, byte ptr[esi+6]
  mov   bl, byte ptr[edi+6]
  movd  mm5, eax
  por   mm0, mm5
  psllq mm1, 32
  movd  mm6, ebx
  por   mm1, mm6
  movq  mm2, mm0   ; copy source
  pcmpeqb mm0, mm4 ; create mask
  pand  mm1, mm0   ; mask dest
  pandn mm0, mm2   ; mask source
  por   mm1, mm0   ; or them
  movd  eax, mm1
  psrlq mm1,32
  mov   byte ptr[edi+6], al
  movd  dword ptr[edi], mm1
  shr   eax,8
  mov   word ptr[edi+4], ax
  jmp   bltEndMMX8

blt6MMX8:
  movd  mm0, dword ptr[esi]
  mov   ax, word ptr[esi+4]
  mov   bx, word ptr[edi+4]
  movd  mm1, dword ptr[edi]
  psllq mm0, 32
  psllq mm1, 32
  movd  mm5, eax
  por   mm0, mm5
  movd  mm6, ebx
  por   mm1, mm6
  movq  mm2, mm0   ; copy source
  pcmpeqb mm0, mm4 ; create mask
  pand  mm1, mm0   ; mask dest
  pandn mm0, mm2   ; mask source
  por   mm1, mm0   ; or them
  movd  eax, mm1
  psrlq mm1,32
  mov   word ptr[edi+4], ax
  movd  dword ptr[edi], mm1
  jmp   bltEndMMX8

blt5MMX8:
  movd mm0, dword ptr[esi]
  movd mm1, dword ptr[edi]
  movq mm2, mm0    ; copy source
  pcmpeqb mm0, mm4 ; create mask
  pand  mm1, mm0   ; mask dest
  add   esi, 4
  pandn mm0, mm2   ; mask source
  por   mm1, mm0   ; or them
  movd  dword ptr[edi], mm1
  add   edi,4
  jmp   blt1MMX8

blt4MMX8:
  movd mm0, dword ptr[esi]
  movd mm1, dword ptr[edi]
  movq mm2,mm0    ; copy source
  pcmpeqb mm0,mm4 ; create mask
  pand  mm1,mm0   ; mask dest
  pandn mm0,mm2   ; mask source
  por   mm1,mm0   ; or them
  movd  dword ptr [edi], mm1 ; write back result
  jmp   bltEndMMX8
;
; loading a dword into mm0/mm1 might be faster for 3-2...
;
blt3MMX8:
  mov  ax, word ptr [esi]
  mov  bx, word ptr [edi]
  shl  eax,8  ; 3 Pixel left to blit
  shl  ebx,8  ; so shift the buffers
  mov  al,byte ptr[esi+2]
  mov  bl,byte ptr[edi+2]
  movd mm0,eax
  movd mm1,ebx
  movq mm2,mm0
  pcmpeqb mm0,mm4 ; create mask
  pand  mm1,mm0   ; mask dest
  pandn mm0,mm2   ; mask source
  por   mm1,mm0   ; or them
  movd  eax, mm1  ; write back result
  mov   byte ptr[edi+2], al
  shr   eax, 8
  mov   word ptr[edi], ax
  jmp bltEndMMX8

blt2MMX8:
  mov  al, byte ptr [esi]
  cmp  al, dl
  je   blt1aMMX8
  mov  byte ptr [edi], al
;  mov  bl, byte ptr [esi+1]
;  cmp  bl, dl
;  je   bltEndMMX8
;  mov  byte ptr [edi+1], bl
;  jmp  bltEndMMX8
blt1aMMX8:
  add esi, 1
  add edi, 1
blt1MMX8:
  mov  al, byte ptr [esi]
  cmp  al, dl
  je   bltEndMMX8
  mov  byte ptr [edi], al

bltEndMMX8:
  pop  edx
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret
_BlitColorKey8MMX ENDP


    PUBLIC  _BlitColorKey16MMX
; Now the same as BlitColorKey16 now with MMX
;void BlitColorKey16MMX(char *dest, char *src, ULONG key, ULONG linesize)
_BlitColorKey16MMX PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  ecx
  push  edx

  mov  edx, dword ptr [ebp+16]    ; colorkey
  mov  edi, dword ptr [ebp+8]     ; dest
  mov  ecx, dword ptr [ebp+20]    ; linesize in pixel!

  mov  eax, edx
  shl  edx, 16;
  mov  dx, ax                     ; extend colorKey to 32 bit

  mov  esi, dword ptr [ebp+12]    ; src
  mov  eax, ecx                   ; copy of linesize
  shr  ecx,2
  movd mm4, edx
  jz BltRemain16

  movd mm5,edx                    ; Extend colorkey to 64 Bit
  psllq mm4,32
  por mm4,mm5

bltLoopMMX16:
  movq mm0,qword ptr [esi]  ; get source dword
  movq mm1,qword ptr [edi]  ; get destination
  movq mm2,mm0              ; copy source
  pcmpeqw mm0,mm4           ; create mask in mm0
  pand  mm1,mm0             ; mask dest
  add   esi, 8              ; point to next source qword
  pandn mm0,mm2             ; NOT mask AND source
  por   mm1,mm0             ; or them
  movq qword ptr [edi], mm1 ; write back result
  add edi, 8
  dec ecx
  jnz bltLoopMMX16

BltRemain16:
  and  eax,3
  jmp  ds:JumpTable[eax*4]

align 4

JumpTable:
  dd offset cs:bltEndMMX16
  dd offset cs:blt1MMX16
  dd offset cs:blt2MMX16
  dd offset cs:blt3MMX16
align 2

blt3MMX16:
  movd mm0, dword ptr[esi]
  movd mm1, dword ptr[edi]
  movq mm2,mm0    ; copy source
  add esi,4
  pcmpeqw mm0,mm4 ; create mask 16 bit
  pand    mm1,mm0 ; mask dest
  pandn   mm0,mm2 ; mask source
  add edi,4
  por   mm1,mm0 ; or them
  movd  dword ptr[edi-4], mm1
  jmp blt1MMX16

blt2MMX16:
  movd mm0, dword ptr[esi]
  movd mm1, dword ptr[edi]
  movq mm2,mm0   ; copy source
  pcmpeqw mm0,mm4 ; create mask 16 bit
  pand  mm1,mm0 ; mask dest
  pandn mm0,mm2             ; mask source
  por   mm1,mm0 ; or them
  movd dword ptr [edi], mm1 ; write back result
  jmp bltEndMMX16

blt1MMX16:
  mov  ax, word ptr [esi] ; cmov ?
  cmp ax,dx
  je  bltEndMMX16
  mov  word ptr [edi], ax


bltEndMMX16:
  pop  edx
  pop  ecx
  pop  esi
  pop  edi
  pop  ebp
  ret
_BlitColorKey16MMX ENDP

;
; extern void __cdecl BltTransSrcRecMMX(PBYTE dest, PBYTE src, ULONG ulBltWidth,ULONG ulBltHeight
;                                       ULONG ulDestPitch, ULONG ulSrcPitch, ULONG ulTransCol);

    PUBLIC _BltTransSrcRecMMX
_BltTransSrcRecMMX PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx
  push  edx

EndTSBlt:
  pop  edx
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret

_BltTransSrcRecMMX ENDP


    PUBLIC _BltRec
;
; extern void __cdecl BltRec(PBYTE dest, PBYTE src, ULONG ulBltWidth,ULONG ulBltHeight
;                           ULONG ulDestPitch, ULONG ulSrcPitch);
_BltRec   PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx
  push  edx

  mov  ecx, dword ptr [ebp+16]    ; U ulBltWidth
  mov  esi, dword ptr [ebp+12]    ; V src
  mov  ebx, ecx                   ; U
  mov  edx, dword ptr [ebp+20]    ; V ulBltHeight
  and  ebx, 0Fh                   ; U ebx = # of bytes < 16
  mov  edi, dword ptr [ebp+8]     ; V dest
  cmp  edx, 0
  jz BltRecEnd                    ; height is zero so done
  shr  ecx, 4                     ; U
  jz   SmallBlt                   ; Small (width < 16) rectangle done in special case
  test ebx, ebx
  jnz  ComplexBlt                 ; ulBltWidth mod 16  is not 0

;
; Blitwidth is an multiple of 16
;
  mov  ebx, dword ptr [ebp+24]    ; ulDestPitch
  mov  eax, dword ptr [ebp+28]    ; ulSrcPitch
  sub  ebx, dword ptr [ebp+16]    ; adjust both widths
  sub  eax, dword ptr [ebp+16]
  mov  dword ptr [ebp+28], eax    ; store adjusted SrcPitch
  mov  eax, ecx
LineLoop:
  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  dec eax
  jz  LineLoop
  dec edx
  jz BltRecEnd
  add ESI, dword ptr[ebp+28]
  add EDI, ebx
  mov  eax, ecx
  jmp LineLoop

SmallBlt:
  mov  eax, dword ptr [ebp+28]    ; ulSrcPitch
  mov  ecx, dword ptr [ebp+24]    ; ulDestPitch
  jmp  ds:SmallJmpTable[ebx*4]
SmallJmpTable:
 dd cs:offset BltRecEnd ; BlitWidth is 0 done
 dd cs:offset Rec1
 dd cs:offset Rec2
 dd cs:offset Rec3
 dd cs:offset Rec4
 dd cs:offset Rec5
 dd cs:offset Rec6
 dd cs:offset Rec7
 dd cs:offset Rec8
 dd cs:offset Rec9
 dd cs:offset Rec10
 dd cs:offset Rec11
 dd cs:offset Rec12
 dd cs:offset Rec13
 dd cs:offset Rec14
 dd cs:offset Rec15

;One Pixel wide

Rec1:
 cmp edx,4
 jb Rec1_0123
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 sub edx,4
 jnz Rec1
 jmp BltRecEnd
Rec1_0123:
 cmp edx,2
 jz Rec1_2
 jb Rec1_01
; Must be 3 lines left
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 mov byte ptr [edi], bl
 jmp BltRecEnd
Rec1_2:
 mov bl, byte ptr [esi]
 add esi,eax
 mov byte ptr [edi], bl
 add edi,ecx
 mov bl, byte ptr [esi]
 mov byte ptr [edi], bl
 jmp BltRecEnd
Rec1_01:
 test edx,edx
 jz BltRecEnd
 mov bl, byte ptr [esi]
 mov byte ptr [edi], bl
 jmp BltRecEnd

;2 Pixel Wide

Rec2:
 cmp edx,4
 jb Rec2_0123
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 sub edx, 4
 jnz Rec2
 jmp BltRecEnd

Rec2_0123:
 cmp edx,2
 jz Rec2_2
 jb Rec2_01
;3 lines left
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 mov word ptr [edi], bx
 jmp BltRecEnd
Rec2_2:
 mov bx, word ptr [esi]
 add esi,eax
 mov word ptr [edi], bx
 add edi,ecx
 mov bx, word ptr [esi]
 mov word ptr [edi], bx
 jmp BltRecEnd
Rec2_01:
 test edx,edx
 jz BltRecEnd
 mov bx, word ptr [esi]
 mov word ptr [edi], bx
 jmp BltRecEnd

; 3 Pixel Wide must check if it's better to read 4 bytes as
; Intel might stall on reading 2 and 1 byte, but this takes more care as we
; could create a pagefault on the last 3 pixel

Rec3:
 cmp edx,4
 jb Rec3_0123
 push edx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 pop edx
 sub edx,4
 jnz Rec3
 jmp BltRecEnd

Rec3_0123:
 cmp edx,2
 jz Rec3_2
 jb Rec3_01
; Must be 3 lines left
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 jmp BltRecEnd
Rec3_2:
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 add esi,eax
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 add edi,ecx
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 jmp BltRecEnd
Rec3_01:
 test edx,edx
 jz BltRecEnd
 mov bx, word ptr [esi]
 mov dl, byte ptr [esi+2]
 mov word ptr [edi], bx
 mov byte ptr [edi+2], dl
 jmp BltRecEnd

; 4 Pixel Wide

Rec4:
 cmp edx,4
 jb Rec4_0123
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 sub edx ,4
 jnz Rec4
 jmp BltRecEnd

Rec4_0123:
 cmp edx,2
 jz Rec2_2
 jb Rec2_01
;3 lines left
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dword ptr [edi], ebx
 jmp BltRecEnd
Rec4_2:
 mov ebx, dword ptr [esi]
 add esi,eax
 mov dword ptr [edi], ebx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dword ptr [edi], ebx
 jmp BltRecEnd
Rec4_01:
 test edx,edx
 jz BltRecEnd
 mov ebx, dword ptr [esi]
 mov dword ptr [edi], ebx
 jmp BltRecEnd

; 5 Pixel Wide

Rec5:
 cmp edx,4
 jb Rec5_0123
 push edx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec5
 jmp BltRecEnd
Rec5_0123:
 cmp edx,2
 jz Rec5_2
 jb Rec5_01
; Must be 3 lines left
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 jmp BltRecEnd
Rec5_2:
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 jmp BltRecEnd
Rec5_01:
 test edx,edx
 jz BltRecEnd
 mov ebx, dword ptr [esi]
 mov dl, byte ptr [esi+4]
 mov dword ptr [edi], ebx
 mov byte ptr [edi+4], dl
 jmp BltRecEnd

; 6 Pixel Wide

Rec6:
 cmp edx,4
 jb Rec6_0123
 push edx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec6
 jmp BltRecEnd
Rec6_0123:
 cmp edx,2
 jz Rec6_2
 jb Rec6_01
; Must be 3 lines left
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 jmp BltRecEnd
Rec6_2:
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 add esi,eax
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 jmp BltRecEnd
Rec6_01:
 test edx,edx
 jz BltRecEnd
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 jmp BltRecEnd

; 7 Pixel Wide

Rec7:
 cmp edx,4
 jb Rec6_0123
 push edx
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 pop edx
 sub edx ,4
 jnz Rec7
 jmp BltRecEnd
Rec7_0123:
 cmp edx,2
 jz Rec7_2
 jb Rec7_01
; Must be 3 lines left
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 mov byte ptr[edi+6],bl
 jmp BltRecEnd
Rec7_2:
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 add esi,eax
 mov byte ptr[edi+6],bl
 add edi,ecx
 xor ebx,ebx ; clear ebx to avoid stalls
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 mov byte ptr[edi+6],bl
 jmp BltRecEnd
Rec7_01:
 test edx,edx
 jz BltRecEnd
 mov ebx, dword ptr [esi]
 mov dx, word ptr [esi+4]
 mov dword ptr [edi], ebx
 mov word ptr [edi+4], dx
 mov bl, byte ptr[esi+6]
 mov byte ptr[edi+6],bl
 jmp BltRecEnd

; 8 Pixel Wide

Rec8:
 cmp edx,4
 jb Rec8_0123
 push edx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec8
 jmp BltRecEnd

Rec8_0123:
 cmp edx,2
 jz Rec8_2
 jb Rec8_01
;3 lines left
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 jmp BltRecEnd
Rec8_2:
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 add esi,eax
 add edi,ecx
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 jmp BltRecEnd
Rec8_01:
 test edx,edx
 jz BltRecEnd
 mov ebx, dword ptr [esi]
 mov edx, dword ptr [esi+4]
 mov dword ptr [edi], ebx
 mov dword ptr [edi+4], edx
 jmp BltRecEnd

; 9 Pixel Wide

Rec9:
 cmp edx,4
 jb Rec9_0123
 push edx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec9
 jmp BltRecEnd

Rec9_0123:
 cmp edx,2
 jz Rec9_2
 jb Rec9_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 jmp BltRecEnd
Rec9_2:
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 jmp BltRecEnd
Rec9_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov bl, byte ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov byte ptr [edi+8], bl
 jmp BltRecEnd

; 10 Pixel Wide

Rec10:
 cmp edx,4
 jb Rec10_0123
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 sub edx ,4
 jnz Rec10
 jmp BltRecEnd

Rec10_0123:
 cmp edx,2
 jz Rec10_2
 jb Rec10_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 jmp BltRecEnd
Rec10_2:
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 jmp BltRecEnd
Rec10_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 jmp BltRecEnd

; 11 Pixel Wide

Rec11:
 cmp edx,4
 jb Rec11_0123
 push edx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec10
 jmp BltRecEnd

Rec11_0123:
 cmp edx,2
 jz Rec11_2
 jb Rec11_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 jmp BltRecEnd
Rec11_2:
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 jmp BltRecEnd
Rec11_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov bx, word ptr [esi+8]
 mov dl, byte ptr [esi+10]
 FSTP QWORD PTR [EDI]
 mov word ptr [edi+8], bx
 mov byte ptr [edi+10], dl
 jmp BltRecEnd

; 12 Pixel Wide

Rec12:
 cmp edx,4
 jb Rec12_0123
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 sub edx ,4
 jnz Rec12
 jmp BltRecEnd

Rec12_0123:
 cmp edx,2
 jz Rec12_2
 jb Rec12_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 jmp BltRecEnd
Rec12_2:
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 jmp BltRecEnd
Rec12_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 jmp BltRecEnd

; 13 Pixel Wide

Rec13:
 cmp edx,4
 jb Rec13_0123
 push edx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec13
 jmp BltRecEnd

Rec13_0123:
 cmp edx,2
 jz Rec13_2
 jb Rec13_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 jmp BltRecEnd
Rec13_2:
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 jmp BltRecEnd
Rec13_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dl, byte ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov byte ptr [edi+12], dl
 jmp BltRecEnd

; 14 Pixel Wide

Rec14:
 cmp edx,4
 jb Rec14_0123
 push edx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec14
 jmp BltRecEnd

Rec14_0123:
 cmp edx,2
 jz Rec14_2
 jb Rec14_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 jmp BltRecEnd
Rec14_2:
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 add esi,eax
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 jmp BltRecEnd
Rec14_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov word ptr [edi+12], dx
 jmp BltRecEnd

; 15 Pixel Wide

Rec15:
 cmp edx,4
 jb Rec15_0123
 push edx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 pop edx
 sub edx ,4
 jnz Rec15
 jmp BltRecEnd

Rec15_0123:
 cmp edx,2
 jz Rec15_2
 jb Rec15_01
;3 lines left
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 mov byte ptr[edi+14], bl
 jmp BltRecEnd
Rec15_2:
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 add esi,eax
 mov byte ptr[edi+14], bl
 add edi,ecx
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 mov byte ptr[edi+14], bl
 jmp BltRecEnd
Rec15_01:
 test edx,edx
 jz BltRecEnd
 FLD QWORD PTR [ESI]
 mov ebx, dword ptr [esi+8]
 mov dx, word ptr [esi+12]
 FSTP QWORD PTR [EDI]
 mov dword ptr [edi+8], ebx
 mov bl, byte ptr[esi+14]
 mov word ptr [edi+12], dx
 mov byte ptr[edi+14], bl
 jmp BltRecEnd


ComplexBlt:
 ; Blit first the even rect then the rest

 push dword ptr [ebp+28]    ; ulSrcPitch
 push dword ptr [ebp+24]    ; ulDestPitch
 push edx
 shl  ecx,4
 push ecx
 push esi
 push edi
 call _BltRec
 sub  esp,24
 add  esi,ecx
 add  edi,ecx
 push dword ptr [ebp+28]    ; ulSrcPitch
 push dword ptr [ebp+24]    ; ulDestPitch
 push edx
 push ebx
 push esi
 push edi
 call _BltRec
 sub  esp,24

BltRecEnd:
  pop  edx
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret
_BltRec   ENDP


    PUBLIC _CPUHasMMX
;
; int __cdecl CPUHasMMX()
; returns:
;   0 = NoMMX
;   1 = MMX
;   2 = MMX+CMov instuction

_CPUHasMMX PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  ebx
  push  ecx
  push  edx

  pushfd
  pop  eax
  mov  ebx ,eax
  xor  eax, 00200000h
  push eax
  popfd
  pushfd
  pop  eax
  sub  eax,ebx
  jz   Return  ; No CPUID => No MMX => return 0 in eax;
  mov eax, 1
  CPUID
  test edx,00800000h ; MMX Bit Set ?
  jz Return
  mov eax, 1
  test edx,00008000h ; Conditonal Mov Bit Set ?
  jz Return
  inc eax
Return:
;  mov  eax, 0 ; pretend no MMX is available
  pop  edx
  pop  ecx
  pop  ebx
  pop  esi
  pop  edi
  pop  ebp
  ret
_CPUHasMMX   ENDP

    PUBLIC _MemFlip

;
; memcpy via FLD / FSTP MMX might even be faster but
; not present on every system
; to maximize the speed we copy 64 bytes in each loop
; and after the loop the rest left
;
;
;void __cdecl MemFlip(PBYTE dest, PBYTE src, ULONG Size);

_MemFlip PROC NEAR
  push  ebp
  mov   ebp, esp
  push  edi
  push  esi
  push  eax
  push  ebx
  push  ecx

  mov eax , dword ptr [ebp+16]    ; Size of Buffer
  mov edi , dword ptr [ebp+8]     ; Destination
  mov ebx , eax
  mov esi , dword ptr [ebp+12]    ; SourcePointer

  and ebx , 0000003Fh             ; Calc leftover bytes
  shr eax , 5                     ; Calc Loops

  jz COPYREMAIN                   ; Less then 64 to copy
ALIGN 4

Loop64:
  FLD QWORD PTR [ESI]      ; 1
  FLD QWORD PTR [ESI+8]    ; 2
  FXCH                     ; Doesn't take any clocks
  FSTP QWORD PTR [EDI]     ; 3,4 Clocks
  FSTP QWORD PTR [EDI+8]   ; 5,6
  ADD ESI,16               ; 7 U Integer instruction can be executed parallel
  ADD EDI,16               ; 7 V Total clocks for copying 16 byte 7 clocks Rep Movs needs 20! for each 16 byte + 13 setup
  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  inc eax
  jnz Loop64

COPYREMAIN:
  test ebx, ebx ; something left ?
  jz EndOffFlip

  test ebx, 00000020h; at least 32 bytes left ?
  jz Test16

  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  sub ebx, 00000020h
  jz EndOffFlip

Test16:

  test ebx, 00000010h; at least 16 bytes left ?
  jb Test8

  FLD QWORD PTR [ESI]
  FLD QWORD PTR [ESI+8]
  FXCH
  FSTP QWORD PTR [EDI]
  FSTP QWORD PTR [EDI+8]
  ADD ESI,16
  ADD EDI,16
  sub ebx, 00000010h
  jz EndOffFlip
Test8:

  test ebx, 00000008h; at least 8 bytes left ?
  jb Test4
  mov eax,[esi]
  mov ecx,[esi+4]
  mov [edi],eax
  mov [edi+4],ecx
  add esi, 8
  add edi, 8
  sub ebx, 8
  jz EndOffFlip

Test4:
  test ebx, 00000004h; at least 4 bytes left ?
  jb Test2
  mov eax,[esi]
  sub ebx, 4
  mov [edi],eax
  add esi, 4
  add edi, 4
  test ebx, ebx ; something left ?
  jz EndOffFlip

Test2:
  test ebx, 00000002h
  jb Copy1
  mov ax,[esi]
  sub ebx,2
  mov [edi],ax
  add esi,2
  add edi,2
  test ebx,ebx
  jz EndOffFlip

Copy1:
  mov al,[esi]
  mov [edi],al

EndOffFlip:
  pop  ecx
  pop  ebx
  pop  eax
  pop  esi
  pop  edi
  pop  ebp
  ret
_MemFlip   ENDP

CODE32          ENDS

                END
