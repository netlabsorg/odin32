; $Id: asmutil.asm,v 1.2 2001-05-31 07:14:00 sandervl Exp $

;
; asmutil.asm utility functions for typelib::invoke
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

    PUBLIC  _invokeStdCallDouble
;double CDECL invokeStdCallDouble(PVOID function, int paramsize, PVOID pStack);
_invokeStdCallDouble PROC NEAR
              push ebp
              mov  ebp, esp
              push ecx
              push ebx

              mov eax, dword ptr [ebp+16] ;pStack
              mov ecx, dword ptr [ebp+12] ; paramsize (in dwords)

pushparam:
              mov ebx, dword ptr [eax]
              push ebx
              sub eax, 4

              dec ecx
              jnz pushparam
              
              call dword ptr [ebp+8] ;call function

              pop ebx
              pop ecx
              pop ebp
              ret
_invokeStdCallDouble ENDP

    PUBLIC  _invokeStdCallDword
;DWORD  CDECL invokeStdCallDword(PVOID function, int paramsize, PVOID pStack);
_invokeStdCallDword  PROC NEAR
              push ebp
              mov  ebp, esp
              push ecx
              push ebx

              mov eax, dword ptr [ebp+16] ;pStack
              mov ecx, dword ptr [ebp+12] ; paramsize (in dwords)

pushparam:
              mov ebx, dword ptr [eax]
              push ebx
              sub eax, 4

              dec ecx
              jnz pushparam
              
              call dword ptr [ebp+8] ;call function

              pop ebx
              pop ecx
              pop ebp
              ret
_invokeStdCallDword  ENDP

CODE32          ENDS

                END
