	.cpu cortex-m4
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"test_2.c"
	.global	__aeabi_i2d
	.global	__aeabi_dmul
	.global	__aeabi_d2iz
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r7, lr}
	sub	sp, sp, #12
	add	r7, sp, #0
	movs	r3, #2
	str	r3, [r7, #4]
	ldr	r0, [r7, #4]
	bl	__aeabi_i2d
	adr	r3, .L3
	ldrd	r2, [r3]
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	bl	__aeabi_d2iz
	mov	r3, r0
	str	r3, [r7]
	ldr	r3, [r7]
	mov	r0, r3
	adds	r7, r7, #12
	mov	sp, r7
	@ sp needed
	pop	{r4, r7, pc}
.L4:
	.align	3
.L3:
	.word	1374389535
	.word	1075388088
	.size	main, .-main
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 7.2.1 20170904 (release) [ARM/embedded-7-branch revision 255204]"
