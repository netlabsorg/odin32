        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCpTranslateChar:PROC
        PUBLIC  _WinCpTranslateChar
_WinCpTranslateChar PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   WinCpTranslateChar
    add    esp, 16
    pop    fs
    ret
_WinCpTranslateChar ENDP

CODE32          ENDS

                END
