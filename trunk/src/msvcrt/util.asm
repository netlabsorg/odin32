        .386p
	.model flat
        .code


        align   4
        public  _MSVCRT__EH_prolog

_MSVCRT__EH_prolog:
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
