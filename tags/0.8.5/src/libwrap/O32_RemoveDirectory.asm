        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RemoveDirectory:PROC
        PUBLIC  O32_RemoveDirectory
O32_RemoveDirectory PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RemoveDirectory
    add    esp, 4
    pop    fs
    ret
O32_RemoveDirectory ENDP

CODE32          ENDS

                END
