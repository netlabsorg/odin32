        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinNextChar:PROC
        PUBLIC  _WinNextChar
_WinNextChar PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinNextChar
    add    esp, 16
    pop    fs
    ret
_WinNextChar ENDP

CODE32          ENDS

                END
