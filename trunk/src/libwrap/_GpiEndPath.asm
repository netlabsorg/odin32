        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiEndPath:PROC
        PUBLIC  _GpiEndPath
_GpiEndPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiEndPath
    add    esp, 4
    pop    fs
    ret
_GpiEndPath ENDP

CODE32          ENDS

                END
