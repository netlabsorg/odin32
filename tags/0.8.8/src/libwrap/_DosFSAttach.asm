        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFSAttach:PROC
        PUBLIC  _DosFSAttach
_DosFSAttach PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosFSAttach
    add    esp, 20
    pop    fs
    ret
_DosFSAttach ENDP

CODE32          ENDS

                END
