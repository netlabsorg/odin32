        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplEnumPrinter:PROC
        PUBLIC  _SplEnumPrinter
_SplEnumPrinter PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    call   SplEnumPrinter
    add    esp, 36
    pop    fs
    ret
_SplEnumPrinter ENDP

CODE32          ENDS

                END
