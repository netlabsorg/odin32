        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCpTranslateString:PROC
        PUBLIC  _WinCpTranslateString
_WinCpTranslateString PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinCpTranslateString
    add    esp, 24
    pop    fs
    ret
_WinCpTranslateString ENDP

CODE32          ENDS

                END
