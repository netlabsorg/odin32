/* File generated automatically. Do not edit! */

	.text

	.align 4
	.stabs "__wine_call_from_32_regs:F1",36,0,0,___wine_call_from_32_regs
	.type ___wine_call_from_32_regs,@function
	.globl ___wine_call_from_32_regs
___wine_call_from_32_regs:
	pushl %ebp
	movl %esp,%ebp
 	leal -332(%esp), %esp
	movl %eax,-156(%ebp)
	pushfl
	popl %eax
	movl %eax,-140(%ebp)
	movl 0(%ebp),%eax
	movl %eax,-152(%ebp)
	movl %ebx,-168(%ebp)
	movl %ecx,-160(%ebp)
	movl %edx,-164(%ebp)
	movl %esi,-172(%ebp)
	movl %edi,-176(%ebp)
	xorl %eax,%eax
	movw %cs,%ax
	movl %eax,-144(%ebp)
	movw %es,%ax
	movl %eax,-184(%ebp)
	movw %fs,%ax
	movl %eax,-188(%ebp)
	movw %gs,%ax
	movl %eax,-192(%ebp)
	movw %ss,%ax
	movl %eax,-132(%ebp)
	movw %ds,%ax
	movl %eax,-180(%ebp)
	movw %ax,%es
	movl $0x10007,%eax
	movl %eax,-332(%ebp)
	movl 8(%ebp),%eax
	movl %eax,-148(%ebp)
	movl 4(%ebp),%ebx
	pushl %esp
	movzbl 4(%ebx),%ecx
	jecxz 1f
	subl %ecx,%esp
	leal 12(%ebp),%esi
	movl %esp,%edi
	shrl $2,%ecx
	cld
	rep
	movsl
1:	movzbl 5(%ebx),%eax
	leal 12(%ebp,%eax),%eax
	movl %eax,-136(%ebp)
	call *0(%ebx)
	movl -136(%ebp),%edx
	movl -148(%ebp),%eax
	movl %eax,-4(%edx)
	movl -152(%ebp),%eax
	movl %eax,-8(%edx)
	movl -184(%ebp),%eax
	movw %ax,%es
	movl -188(%ebp),%eax
	movw %ax,%fs
	movl -192(%ebp),%eax
	movw %ax,%gs
	movl -176(%ebp),%edi
	movl -172(%ebp),%esi
	movl -164(%ebp),%edx
	movl -160(%ebp),%ecx
	movl -168(%ebp),%ebx
	movl -140(%ebp),%eax
	pushl %eax
	popfl
	movl -156(%ebp),%eax
	movl -136(%ebp),%ebp
        leal -8(%ebp),%esp
	popl %ebp
	ret
	.stabs "",100,0,0,.Letext
.Letext:

