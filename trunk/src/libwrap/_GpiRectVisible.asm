        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiRectVisible:PROC
        PUBLIC  _GpiRectVisible
_GpiRectVisible PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiRectVisible
    add    esp, 8
    pop    fs
    ret
_GpiRectVisible ENDP

CODE32          ENDS

                END
