        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxRegisterFunctionDll:PROC
        PUBLIC  _RexxRegisterFunctionDll
_RexxRegisterFunctionDll PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   RexxRegisterFunctionDll
    add    esp, 12
    pop    fs
    ret
_RexxRegisterFunctionDll ENDP

CODE32          ENDS

                END
