        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinQueryObject:PROC
        PUBLIC  _WinQueryObject
_WinQueryObject PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinQueryObject
    add    esp, 4
    pop    fs
    ret
_WinQueryObject ENDP

CODE32          ENDS

                END
