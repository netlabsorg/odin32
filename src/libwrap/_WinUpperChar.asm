        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinUpperChar:PROC
        PUBLIC  _WinUpperChar
_WinUpperChar PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinUpperChar
    add    esp, 16
    pop    fs
    ret
_WinUpperChar ENDP

CODE32          ENDS

                END
