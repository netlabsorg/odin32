        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxRegisterSubcomExe:PROC
        PUBLIC  _RexxRegisterSubcomExe
_RexxRegisterSubcomExe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   RexxRegisterSubcomExe
    add    esp, 12
    pop    fs
    ret
_RexxRegisterSubcomExe ENDP

CODE32          ENDS

                END
