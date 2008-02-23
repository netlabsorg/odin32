        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgPostTransferMsg:PROC
        PUBLIC  _DrgPostTransferMsg
_DrgPostTransferMsg PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   DrgPostTransferMsg
    add    esp, 24
    pop    fs
    ret
_DrgPostTransferMsg ENDP

CODE32          ENDS

                END
