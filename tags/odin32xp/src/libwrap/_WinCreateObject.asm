        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCreateObject:PROC
        PUBLIC  _WinCreateObject
_WinCreateObject PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinCreateObject
    add    esp, 20
    pop    fs
    ret
_WinCreateObject ENDP

CODE32          ENDS

                END
