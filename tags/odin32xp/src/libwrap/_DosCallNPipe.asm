        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCallNPipe:PROC
        PUBLIC  _DosCallNPipe
_DosCallNPipe PROC NEAR
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
    call   DosCallNPipe
    add    esp, 28
    pop    fs
    ret
_DosCallNPipe ENDP

CODE32          ENDS

                END
