        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinDestroyObject:PROC
        PUBLIC  _WinDestroyObject
_WinDestroyObject PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinDestroyObject
    add    esp, 4
    pop    fs
    ret
_WinDestroyObject ENDP

CODE32          ENDS

                END
