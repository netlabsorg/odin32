        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRegisterClass:PROC
        PUBLIC  _WinRegisterClass
_WinRegisterClass PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinRegisterClass
    add    esp, 20
    pop    fs
    ret
_WinRegisterClass ENDP

CODE32          ENDS

                END
