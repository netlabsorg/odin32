        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DevQueryCaps:PROC
        PUBLIC  _DevQueryCaps
_DevQueryCaps PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DevQueryCaps
    add    esp, 16
    pop    fs
    ret
_DevQueryCaps ENDP

CODE32          ENDS

                END
