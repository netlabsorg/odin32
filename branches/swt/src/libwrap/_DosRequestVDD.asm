        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosRequestVDD:PROC
        PUBLIC  _DosRequestVDD
_DosRequestVDD PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   DosRequestVDD
    add    esp, 28
    pop    fs
    ret
_DosRequestVDD ENDP

CODE32          ENDS

                END
