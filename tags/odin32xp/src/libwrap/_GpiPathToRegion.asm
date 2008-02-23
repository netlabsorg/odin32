        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPathToRegion:PROC
        PUBLIC  _GpiPathToRegion
_GpiPathToRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiPathToRegion
    add    esp, 12
    pop    fs
    ret
_GpiPathToRegion ENDP

CODE32          ENDS

                END
