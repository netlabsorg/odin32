/** @file
 *
 * Minimalistic application entry point.
 *
 * This gets control when the executable is loaded.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

    .globl  WEAK$ZERO

WEAK$ZERO = 0

    .globl __text
    .globl ___main


    .text

/*
 * Entry point.
 *
 * It calls the function that must be defined as
 * <code>
 *      extern "C" int simple_main(void)
 * <code>
 * and returns its return code to the system.
 */
__text:

    cld
    call    _simple_main
    ret

/*
 * GCC always generates a call to ___main from _main, so use a dummy
 * funciton.
 */
___main:
    ret


    .data

/*
 * force creation of the data segment to keep the assumed order of
 * objects in the executable (CODE, DATA/BSS, STACK)
 */
	.long 0
