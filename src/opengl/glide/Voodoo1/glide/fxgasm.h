SST_CHIP_MASK = 03c00h

;----------------------------------------------------------------------
; Assembler offsets for SSTREGS struct
;----------------------------------------------------------------------

FVAX			=        136
FVAY			=        140
FVBX			=        144
FVBY			=        148
FVCX			=        152
FVCY			=        156
FTRIANGLECMD		=        256

FDPDX_OFFSET		=         32
FDPDY_OFFSET		=         64

;----------------------------------------------------------------------
; Assembler offsets for GC struct
;----------------------------------------------------------------------

base_ptr		=          0
reg_ptr			=          4
lfb_ptr			=         12
cull_mode		=        400
dataList		=         24
fifoFree		=        408
SIZEOF_GrState		=        256
SIZEOF_GrHwConfiguration	=        148
SIZEOF_GC		=     201556


;----------------------------------------------------------------------
; Assembler offsets for GlideRoot struct
;----------------------------------------------------------------------

p6Fencer		=          0
current_sst		=          4
CPUType			=          8
curGC			=         12
curTriSize		=         16
trisProcessed		=        124
trisDrawn		=        128
SIZEOF_GlideRoot	=     806520


;----------------------------------------------------------------------
; Assembler offsets for dataList struct
;----------------------------------------------------------------------

dl_i			=          0
dl_addr			=          4
SIZEOF_dataList		=          8

