        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DevEscape:PROC
        PUBLIC  _DevEscape
_DevEscape PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DevEscape
    add    esp, 24
    pop    fs
    ret
_DevEscape ENDP

CODE32          ENDS

                END
