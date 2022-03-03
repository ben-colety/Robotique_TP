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
	.file	"test.c"
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.fpu softvfp
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	sub	sp, sp, #28
	add	r7, sp, #0
	movs	r3, #0
	str	r3, [r7, #8]
	movs	r3, #5
	str	r3, [r7, #4]
	movs	r3, #10
	str	r3, [r7, #8]
	movs	r3, #0
	str	r3, [r7, #20]
	b	.L2
.L5:
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L3
.L4:
	ldr	r2, [r7, #20]
	ldr	r3, [r7, #16]
	add	r3, r3, r2
	ldr	r2, [r7, #12]
	add	r3, r3, r2
	str	r3, [r7, #12]
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L3:
	ldr	r3, [r7, #16]
	cmp	r3, #9
	ble	.L4
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L2:
	ldr	r3, [r7, #20]
	cmp	r3, #9
	ble	.L5
	ldr	r3, [r7, #12]
	mov	r0, r3
	adds	r7, r7, #28
	mov	sp, r7
	@ sp needed
	pop	{r7}
	bx	lr
	.size	main, .-main
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 7.2.1 20170904 (release) [ARM/embedded-7-branch revision 255204]"
