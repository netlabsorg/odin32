        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinOffsetRect:PROC
        PUBLIC  _WinOffsetRect
_WinOffsetRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinOffsetRect
    add    esp, 16
    pop    fs
    ret
_WinOffsetRect ENDP

CODE32          ENDS

                END
