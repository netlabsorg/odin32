        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQmOpen:PROC
        PUBLIC  _SplQmOpen
_SplQmOpen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   SplQmOpen
    add    esp, 12
    pop    fs
    ret
_SplQmOpen ENDP

CODE32          ENDS

                END
