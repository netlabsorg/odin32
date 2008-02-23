        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplPurgeQueue:PROC
        PUBLIC  _SplPurgeQueue
_SplPurgeQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   SplPurgeQueue
    add    esp, 8
    pop    fs
    ret
_SplPurgeQueue ENDP

CODE32          ENDS

                END
