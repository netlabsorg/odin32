        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplCopyJob:PROC
        PUBLIC  _SplCopyJob
_SplCopyJob PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplCopyJob
    add    esp, 24
    pop    fs
    ret
_SplCopyJob ENDP

CODE32          ENDS

                END
