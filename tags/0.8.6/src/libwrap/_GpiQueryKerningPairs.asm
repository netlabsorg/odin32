        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryKerningPairs:PROC
        PUBLIC  _GpiQueryKerningPairs
_GpiQueryKerningPairs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiQueryKerningPairs
    add    esp, 12
    pop    fs
    ret
_GpiQueryKerningPairs ENDP

CODE32          ENDS

                END
