; **********************************************************************
; Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de)
; Use at your own risk! No Warranty! The author is not responsible for
; any damage or loss of data caused by proper or improper use of this
; device driver.
; **********************************************************************
;
; compile with ALP
;
  TITLE  ICCIO.ASM
  .386
  .387
CODE32  SEGMENT DWORD USE32 PUBLIC 'CODE'
CODE32  ENDS
DATA32  SEGMENT DWORD USE32 PUBLIC 'DATA'
DATA32  ENDS
CONST32  SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST32  ENDS
BSS32  SEGMENT DWORD USE32 PUBLIC 'BSS'
BSS32  ENDS
DGROUP  GROUP CONST32, BSS32, DATA32
  ASSUME  CS:FLAT, DS:FLAT, SS:FLAT, ES:FLAT
DATA32  SEGMENT
DATA32  ENDS
BSS32  SEGMENT
BSS32  ENDS
CONST32  SEGMENT
CONST32  ENDS

DATA32  SEGMENT
ioentry   DWORD  0
gdt    WORD  0
DATA32  ENDS

CODE32  SEGMENT

; performs fast output of a byte to an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  void c_outb1(short port,char data)
;
;
  PUBLIC  c_outb1
  ALIGN  04H
c_outb1  PROC
  MOV  EDX, [ESP+4]    ; get port
  MOV  AL, [ESP+8]    ; get data
  OUT  DX,AL
  RET
c_outb1  ENDP

; performs fast output of a word to an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  void c_outw1(short port,short data)
;
;
  PUBLIC  c_outw1
  ALIGN  04H
c_outw1  PROC
  MOV  EDX, [ESP+4]    ; get port
  MOV  AX, [ESP+8]    ; get data
  OUT  DX,AX
  RET
c_outw1  ENDP

; performs fast output of a longword to an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  void c_outl1(short port,long data)
;
;
  PUBLIC  c_outl1
  ALIGN  04H
c_outl1  PROC
  MOV  EDX, [ESP+4]    ; get port
  MOV  EAX, [ESP+8]    ; get data
  OUT  DX, EAX
  RET
c_outl1  ENDP

; performs fast input of a byte from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  char c_inb1(short port)
;
;
  PUBLIC c_inb1
  ALIGN  04H
c_inb1  PROC
  MOV  EDX, [ESP+4]    ; get port
  IN  AL,DX
  AND  EAX, 000000FFh
  RET
c_inb1  ENDP

; performs fast input of a word from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  short c_inw1(short port)
;
;
  PUBLIC c_inw1
  ALIGN  04H
c_inw1  PROC
  MOV  EDX, [ESP+4]    ; get port
  IN  AX, DX
  AND  EAX, 0000FFFFh    ; mask out word
  RET
c_inw1  ENDP

; performs fast input of a longword from an I/O port
; this routine is intended to be called from gcc C code
;
; Calling convention:
;  lomg c_inl1(short port)
;
;
  PUBLIC c_inl1
  ALIGN  04H
c_inl1  PROC
  MOV  EDX, [ESP+4]    ; get port
  IN  EAX, DX
  RET
c_inl1  ENDP

CODE32  ENDS

;------------------------------------------------------------------------------

; Initialize I/O access via the driver.
; You *must* call this routine once for each *thread* that wants to do
; I/O.
;
; The routine is mainly equivalent to a C routine performing the
; following (but no need to add another file):
;  DosOpen("/dev/fastio$", read, nonexclusive)
;  DosDevIOCtl(device, XFREE86IO, IOGETSEL32)
;  selector -> ioentry+4
;  DosClose(device)
;
; Calling convention:
;  int io_init1(void)
; Return:
;  0 if successful
;  standard APIRET RETurn code if error
;

CONST32  SEGMENT
  ALIGN  04H
devname:
  DB  "/dev/fastio$",0
CONST32  ENDS


CODE32  SEGMENT
  PUBLIC  io_init1
  EXTRN  DosOpen:PROC
  EXTRN  DosClose:PROC
  EXTRN  DosDevIOCtl:PROC
  ALIGN  04H
io_init1  PROC
  PUSH  EBP
  MOV  EBP, ESP  ; standard stack frame
  SUB  ESP, 16    ; reserve memory
        ; -16 = len arg of DosDevIOCtl
        ; -12 = action arg of DosOpen
        ; -8 = fd arg of DosOpen
        ; -2 = short GDT selector arg
  PUSH  0    ; (PEAOP2)NULL
  PUSH  66    ; OPENACCESSREADWRITE|OPENSHAREDENYNONE
  PUSH  1    ; FILEOPEN
  PUSH  0    ; FILENORMAL
  PUSH  0    ; initial size
  LEA  EAX, [EBP-12]  ; Adress of 'action' arg
  PUSH  EAX
  LEA  EAX, [EBP-8]  ; Address of 'fd' arg
  PUSH  EAX
  PUSH  OFFSET FLAT:devname
  CALL  DosOpen    ; call DosOpen
  ADD  ESP, 32    ; cleanup stack frame
  CMP  EAX, 0    ; is return code zero?
  JE  goon    ; yes, proceed
  LEAVE      ; no RETurn error
  RET
  ALIGN  04H
goon:
  LEA  EAX, [EBP-16]  ; address of 'len' arg of DosDevIOCtl
  PUSH  EAX
  PUSH  2    ; sizeof(short)
  LEA  EAX, [EBP-2]  ; address to return the GDT selector
  PUSH  EAX
  PUSH  0    ; no parameter len
  PUSH  0    ; no parameter size
  PUSH  0    ; no parameter address
  PUSH  100    ; function code IOGETSEL32
  PUSH  118    ; category code XFREE6IO
  MOV  EAX,[EBP-8]  ; file handle
  PUSH  EAX
  CALL  DosDevIOCtl  ; perform ioctl
  ADD  ESP, 36    ; cleanup stack
  CMP  EAX, 0    ; is return code = 0?
  JE  ok    ; yes, proceed
  PUSH  EAX    ; was error, save error code
  MOV  EAX, [EBP-8]  ; file handle
  PUSH  EAX
  CALL  DosClose  ; close device
  ADD  ESP,4    ; clean stack
  POP  EAX    ; get error code
  LEAVE      ; return error
  RET

  ALIGN  04H
ok:
  MOV  EAX, [EBP-8]  ; file handle
  PUSH  EAX    ; do normal close
  CALL  DosClose
  ADD  ESP,4    ; clean stack

  MOV  AX, [EBP-2]  ; load gdt selector
  MOV  gdt, AX    ; store in ioentry address selector part
  XOR  EAX, EAX  ; EAX = 0
  MOV  DWORD PTR [ioentry], EAX ; clear ioentry offset part
        ; return code = 0 (in EAX)

        ; now use this function to raise the IOPL
  PUSH  EBX                             ; bird fix
  MOV   EBX,13    ; special function code
  CALL  FWORD PTR [ioentry]  ; CALL intersegment indirect 16:32
  POP   EBX                             ; bird fix

  ; thread should now be running at IOPL=3

  XOR  EAX, EAX  ; return code = 0
  LEAVE      ; clean stack frame
  RET      ; exit
io_init1 ENDP

; void in_init(short)
PUBLIC  io_init2
  ALIGN  04H
io_init2  PROC
  MOV  EAX, dword ptr [ESP+4]
  MOV  gdt, AX    ; store in ioentry address selector part
  XOR  EAX, EAX  ; EAX = 0
  MOV  DWORD PTR [ioentry], EAX ; clear ioentry offset part
        ; return code = 0 (in EAX)

        ; now use this function to raise the IOPL
  PUSH  EBX                             ; bird fix
  MOV   EBX,13    ; special function code
  CALL  FWORD PTR [ioentry]  ; CALL intersegment indirect 16:32
  POP   EBX                             ; bird fix

  XOR  EAX, EAX  ; return code = 0
  ret
io_init2  ENDP

  PUBLIC  io_exit1
  ALIGN  04H
io_exit1  PROC
  push  EBP
  MOV  EBP, ESP  ; stackframe, I am accustomed to this :-)

  MOV  AX, gdt    ; check if ioinit was called once
  OR  AX, AX
  JZ  exerr    ; no gdt entry, so process cannot be at IOPL=3
        ; through this mechanism
  PUSH  EBX                             ; bird fix
  MOV   EBX, 14    ; function code to disable iopl
  CALL  FWORD PTR [ioentry]  ; call intersegment indirect 16:32
  POP   EBX                             ; bird fix

  ; process should now be at IOPL=3 again
  XOR  EAX, EAX  ; ok, RETurn code = 0
  LEAVE
  RET
exerr:  XOR  EAX, EAX  ; not ok, RETurn code = ffffffff
  DEC  EAX
  LEAVE
  RET
io_exit1  ENDP



; for diagnostic only

;;  PUBLIC  psw
;;  ALIGN  04H
;;psw  PROC
;;  PUSHF      ; get the current PSW
;;  POP  EAX    ; into EAX
;;  RET
;;psw  ENDP

CODE32  ENDS
  END
