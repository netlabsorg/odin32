        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCombineRegion:PROC
        PUBLIC  _GpiCombineRegion
_GpiCombineRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiCombineRegion
    add    esp, 20
    pop    fs
    ret
_GpiCombineRegion ENDP

CODE32          ENDS

                END
