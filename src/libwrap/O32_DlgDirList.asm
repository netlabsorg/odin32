        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DlgDirList:PROC
        PUBLIC  O32_DlgDirList
O32_DlgDirList PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_DlgDirList
    add    esp, 20
    pop    fs
    ret
O32_DlgDirList ENDP

CODE32          ENDS

                END
