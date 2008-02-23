        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiImage:PROC
        PUBLIC  _GpiImage
_GpiImage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiImage
    add    esp, 20
    pop    fs
    ret
_GpiImage ENDP

CODE32          ENDS

                END
