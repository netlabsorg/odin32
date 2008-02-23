        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGetResource:PROC
        PUBLIC  _DosGetResource
_DosGetResource PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosGetResource
    add    esp, 16
    pop    fs
    ret
_DosGetResource ENDP

CODE32          ENDS

                END
