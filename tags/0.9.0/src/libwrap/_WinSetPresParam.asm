        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetPresParam:PROC
        PUBLIC  _WinSetPresParam
_WinSetPresParam PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinSetPresParam
    add    esp, 16
    pop    fs
    ret
_WinSetPresParam ENDP

CODE32          ENDS

                END
