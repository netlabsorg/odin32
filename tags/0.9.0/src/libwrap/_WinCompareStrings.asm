        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCompareStrings:PROC
        PUBLIC  _WinCompareStrings
_WinCompareStrings PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   WinCompareStrings
    add    esp, 24
    pop    fs
    ret
_WinCompareStrings ENDP

CODE32          ENDS

                END
