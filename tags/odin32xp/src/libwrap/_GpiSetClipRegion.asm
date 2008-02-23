        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetClipRegion:PROC
        PUBLIC  _GpiSetClipRegion
_GpiSetClipRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetClipRegion
    add    esp, 12
    pop    fs
    ret
_GpiSetClipRegion ENDP

CODE32          ENDS

                END
