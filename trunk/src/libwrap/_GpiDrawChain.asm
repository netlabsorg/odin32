        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDrawChain:PROC
        PUBLIC  _GpiDrawChain
_GpiDrawChain PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiDrawChain
    add    esp, 4
    pop    fs
    ret
_GpiDrawChain ENDP

CODE32          ENDS

                END
