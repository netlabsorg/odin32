        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   RexxRegisterFunctionExe:PROC
        PUBLIC  _RexxRegisterFunctionExe
_RexxRegisterFunctionExe PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   RexxRegisterFunctionExe
    add    esp, 8
    pop    fs
    ret
_RexxRegisterFunctionExe ENDP

CODE32          ENDS

                END
