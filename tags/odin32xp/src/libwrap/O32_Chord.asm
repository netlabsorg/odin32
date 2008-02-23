        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_Chord:PROC
        PUBLIC  O32_Chord
O32_Chord PROC NEAR
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
    call   _O32_Chord
    add    esp, 36
    pop    fs
    ret
O32_Chord ENDP

CODE32          ENDS

                END
