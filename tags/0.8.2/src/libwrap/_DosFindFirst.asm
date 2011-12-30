        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFindFirst:PROC
        PUBLIC  _DosFindFirst
_DosFindFirst PROC NEAR
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
    call   DosFindFirst
    add    esp, 28
    pop    fs
    ret
_DosFindFirst ENDP

CODE32          ENDS

                END
