        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryTextBox:PROC
        PUBLIC  _GpiQueryTextBox
_GpiQueryTextBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryTextBox
    add    esp, 20
    pop    fs
    ret
_GpiQueryTextBox ENDP

CODE32          ENDS

                END
