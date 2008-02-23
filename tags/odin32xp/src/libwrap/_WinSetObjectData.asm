        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetObjectData:PROC
        PUBLIC  _WinSetObjectData
_WinSetObjectData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetObjectData
    add    esp, 8
    pop    fs
    ret
_WinSetObjectData ENDP

CODE32          ENDS

                END
