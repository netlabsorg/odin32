        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinRemovePresParam:PROC
        PUBLIC  _WinRemovePresParam
_WinRemovePresParam PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinRemovePresParam
    add    esp, 8
    pop    fs
    ret
_WinRemovePresParam ENDP

CODE32          ENDS

                END
