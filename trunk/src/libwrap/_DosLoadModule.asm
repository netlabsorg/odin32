        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosLoadModule:PROC
        PUBLIC  _DosLoadModule
_DosLoadModule PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosLoadModule
    add    esp, 16
    pop    fs
    ret
_DosLoadModule ENDP

CODE32          ENDS

                END
