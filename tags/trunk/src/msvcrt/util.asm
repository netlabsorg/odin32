        .386p
 .model flat

CONST32 segment dword use32 public 'CONST'
     align 04h
label1  db "MSVCRT: EH_prolog",0ah, 0h
CONST32 ends

        .code

        extrn   WriteLog:proc
        public  _MSVCRT__EH_prolog

        align   4
_MSVCRT__EH_prolog:
   push        eax
   push        ecx
   push        edx
   pushfd
   push        offset FLAT:label1
   call        WriteLog
   add         esp,04h
   popfd
   pop         edx
   pop         ecx
   pop         eax

   push        0FFh
   push        eax
   mov         eax,fs:[00000000]
   push        eax
   mov         eax,dword ptr [esp+0Ch]
   mov         dword ptr fs:[0],esp
   mov         dword ptr [esp+0Ch],ebp
   lea         ebp,[esp+0Ch]
   push        eax
   ret

        end
