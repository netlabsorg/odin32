        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFindNext:PROC
        PUBLIC  _DosFindNext
_DosFindNext PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosFindNext
    add    esp, 16
    pop    fs
    ret
_DosFindNext ENDP

CODE32          ENDS

                END
