        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiOffsetClipRegion:PROC
        PUBLIC  _GpiOffsetClipRegion
_GpiOffsetClipRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiOffsetClipRegion
    add    esp, 8
    pop    fs
    ret
_GpiOffsetClipRegion ENDP

CODE32          ENDS

                END
