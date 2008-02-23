        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxRegisterExitDll:PROC
        PUBLIC  _RexxRegisterExitDll
_RexxRegisterExitDll PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   RexxRegisterExitDll
    add    esp, 20
    pop    fs
    ret
_RexxRegisterExitDll ENDP

CODE32          ENDS

                END
