        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplReleaseQueue:PROC
        PUBLIC  _SplReleaseQueue
_SplReleaseQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   SplReleaseQueue
    add    esp, 8
    pop    fs
    ret
_SplReleaseQueue ENDP

CODE32          ENDS

                END
