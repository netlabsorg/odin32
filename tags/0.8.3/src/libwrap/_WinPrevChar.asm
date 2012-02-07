        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinPrevChar:PROC
        PUBLIC  _WinPrevChar
_WinPrevChar PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   WinPrevChar
    add    esp, 20
    pop    fs
    ret
_WinPrevChar ENDP

CODE32          ENDS

                END
