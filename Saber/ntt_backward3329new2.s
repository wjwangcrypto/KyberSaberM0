	MACRO 
	radix2ButterflyBackK $opA, $opB, $root3329, $tmp1, $tmp2
	SUBS	$tmp1, $opA, $opB
	ADDS	$opA, $opA, $opB
	
	MULS	$tmp1, $root3329, $tmp1
	UXTB	$opB, $tmp1
	ASRS	$tmp1, $tmp1, #8
;	MOVS	$root3329, #13
	MULS	$opB, $tmp2, $opB
	SUBS	$opB, $opB, $tmp1
	MEND	
	
	MACRO 
	radix2ButterflyBACk_MontPPinv $opA , $opB , $root3329 , $tmp1, $prime, $primeINV
	SUBS	$tmp1, $opA, $opB
	ADDS	$opA, $opA, $opB
	
	MULS	$tmp1, $root3329, $tmp1
	UXTH	$opB, $tmp1
	ASRS	$tmp1, $tmp1, #16
;	LDR		$opB, =-3327
;	LTORG
	MULS	$opB, $primeINV, $opB
	UXTH	$opB, $opB
;	LDR		$opB, =3329
;	LTORG
	MULS	$opB, $prime, $opB
	ASRS	$opB, $opB, #16
	SUBS	$opB, $tmp1, $opB	
	MEND
	
	MACRO 
	radix2ButterflyBACk_MontNewPPinv $opA , $opB , $root3329 , $tmp1, $prime, $primeINV
	SUBS	$tmp1, $opA, $opB
	ADDS	$opA, $opA, $opB
	
	MULS	$tmp1, $root3329, $tmp1
	MOVS	$opB, $tmp1
;	LDR		$opB, =-3327
;	LTORG
	MULS	$opB, $primeINV, $opB
	UXTH	$opB, $opB
;	LDR		$opB, =3329
;	LTORG
	MULS	$opB, $prime, $opB
	SUBS	$opB, $tmp1, $opB
	ASRS	$opB, $opB, #16	
	MEND
	
	MACRO
	barrettPrime $opA, $tmp1, $tmp2
;	MOVS	$tmp2, #157
	LDR		$tmp2, =315
	MOVS	$tmp1, $opA
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, #20
	LDR		$tmp2, =3329
	MULS	$tmp1, $tmp2, $tmp1
	SUBS	$opA, $opA, $tmp1
	MEND
	
	AREA ASM, code, readonly	
	THUMB
	EXPORT NTT_backward3329new2		
		
	IMPORT root3329_inv7_7kRED		
	IMPORT root3329_65_7Mont
	IMPORT root3329_43_7Mont
	IMPORT root3329_21_7Mont
		
NTT_backward3329new2
	PUSH {r0,r1,r2,r3,r4,r5,r6,r7,lr}
	MOV		r4, r8
	PUSH	{r4}
	MOV		r4, r9
	PUSH	{r4}
	MOV		r4, r10
	PUSH	{r4}
	MOV		r4, r11
	PUSH	{r4}
	MOV		r4, r12
	PUSH	{r4}
	
	PUSH	{r1}
NTT_backward3329new2_inv7level	
	LDR		r2, = root3329_inv7_7kRED
	LTORG
	MOVS	r7, #1
	MOV		r10, r7
	MOVS	r6, #13
NTT_backward3329new2_inv7level_outerloop
;	/*level-inv7*/
	MOVS	r5, #0	
	LDRSH 	r3, [r1, r5]
	MOVS	r5, #4
	LDRSH	r4, [r1, r5]
	
	LDRH	r7, [r2, #0]
	radix2ButterflyBackK	r3, r4, r7, r5, r6
	STR 	r3, [r0, #0]
	STR		r4, [r0, #2*4]
	
	MOVS	r5, #2
	LDRSH	r3, [r1, r5]
	MOVS	r5, #6
	LDRSH	r4, [r1, r5]
	radix2ButterflyBackK	r3, r4, r7, r5, r6
	STR		r3, [r0, #1*4]
	STR		r4, [r0, #3*4]
	
	ADDS	r0, r0, #4*4
	ADDS	r1, r1, #4*2
	ADDS	r2, r2, #2
	
	MOV		r7, r10
	ADDS	r7, r7, #1
	MOV		r10, r7
	CMP		r7, #65
	BNE		NTT_backward3329new2_inv7level_outerloop
	
NTT_backward3329new2_65level
	MOVS	r2, #64
	LSLS	r2, #4
	SUBS	r0, r0, r2
	
	LDR 	r1, =root3329_65_7Mont
	MOVS	r7, #1
NTT_backward3329new2_65level_outerloop	
	LDRH	r2, [r1, #0]
	MOV		r10, r2
	LDRH	r2, [r1, #2]
	MOV		r11, r2
	LDRH	r2, [r1, #4]
	MOV		r12, r2
	
	PUSH	{r7}
	PUSH	{r1}
	LDR		r7, =3329
	LDR		r1, =-3327
	
	MOVS	r6, #1
NTT_backward3329new2_65level_innerloop
;	/*level-inv6*/
	LDR 	r3, [r0, #0]
	LDR		r4, [r0, #4*4]
	MOV		r2, r11
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r7, r1
	MOV		r14, r3
	MOV		r8, r4
	
	LDR		r3, [r0, #8*4]
	LDR		r4, [r0, #12*4]
	MOV		r2, r12
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r7, r1
	
;	/*level-inv5*/
	MOV		r9, r4
	MOV		r4, r14	
	
	MOV		r2, r10
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r7, r1
	STR 	r4, [r0, #0]
	STR		r3, [r0, #8*4]

	MOV		r4, r8
	MOV		r3, r9
	
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r7, r1
	STR		r4, [r0, #4*4]
	STR		r3, [r0, #12*4]
	
	ADDS	r0, r0, #1*4
	ADDS	r6, r6, #1
	CMP		r6, #5
	BNE		NTT_backward3329new2_65level_innerloop

	POP		{r1}
	ADDS	r1, r1, #3*2
	
	MOVS	r2, #48
	ADDS	r0, r0, r2
	
	POP		{r7}
	ADDS	r7, r7, #1
	CMP		r7, #17
	BNE		NTT_backward3329new2_65level_outerloop
	
NTT_backward3329new2_43level
	MOVS	r2, #64
	LSLS	r2, #4
	SUBS	r0, r0, r2
		
	LDR 	r1, =root3329_43_7Mont
	MOVS	r7, #1
NTT_backward3329new2_43level_outerloop	
	LDRH	r2, [r1, #0]
	MOV		r10, r2
	LDRH	r2, [r1, #2]
	MOV		r11, r2
	LDRH	r2, [r1, #4]
	MOV		r12, r2
	
	PUSH	{r7}
	PUSH	{r1}
	LDR		r7, =3329
	LDR		r1, =-3327
	
	MOVS	r6, #1
NTT_backward3329new2_43level_innerloop
;	/*level-inv4*/
	LDR 	r3, [r0, #0]
	LDR		r4, [r0, #16*4]
	MOV		r2, r11
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r7, r1
	MOV		r14, r3
	MOV		r8, r4
	
	MOVS	r5, #128
	LDR		r3, [r0, r5]
	
	ADDS	r5, r5, #64
	LDR		r4, [r0, r5]
	MOV		r2, r12
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r7, r1
	
;	/*level-inv3*/
	MOV		r9, r4
	MOV		r4, r14
	
	MOV		r2, r10
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r7, r1
	STR 	r4, [r0, #0]
	MOVS	r5, #128
	STR		r3, [r0, r5]
	
	MOV		r4, r8
	MOV		r3, r9
	
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r7, r1
	STR		r4, [r0, #16*4]
	MOVS	r5, #128
	ADDS	r5, r5, #64
	STR		r3, [r0, r5]
	
	ADDS	r0, r0, #1*4
	ADDS	r6, r6, #1
	CMP		r6, #17
	BNE		NTT_backward3329new2_43level_innerloop
	
	POP		{r1}
	ADDS	r1, r1, #3*2
	
	MOVS	r2, #48
	LSLS	r2, #2
	ADDS	r0, r0, r2
	
	POP		{r7}
	ADDS	r7, r7, #1
	CMP		r7, #5
	BNE		NTT_backward3329new2_43level_outerloop
	
NTT_backward3329new2_21level
	MOVS	r2, #64
	LSLS	r2, #4
	SUBS	r0, r0, r2
	POP		{r1}
	
	LDR 	r5, =root3329_21_7Mont
	LDRH	r2, [r5, #0]
	MOV		r10, r2
	LDRH	r2, [r5, #2]
	MOV		r11, r2
	LDRH	r2, [r5, #4]
	MOV		r12, r2

;	LDR		r1, =-3327
	LDR		r6, =3329
	
	MOVS	r7, #1
NTT_backward3329new2_21level_loop
	PUSH	{r7}
	LDR		r7, =-3327
;	/*level-inv2*/
	LDR 	r3, [r0, #0]
	
	MOVS	r5, #128
	LSLS	r5, #1
	MOV		r9, r5
	LDR		r4, [r0, r5]
	
	MOV		r2, r11
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r6, r7
	MOV		r14, r3
	MOV		r8, r4
	
	MOV		r5, r9
	LSLS	r5, #1
	LDR		r3, [r0, r5]
	
	ADD		r5, r5, r9
	LDR		r4, [r0, r5]
	
	MOV		r2, r12
	radix2ButterflyBACk_MontNewPPinv	r3, r4, r2, r5, r6, r7
	
;	/*level-inv1*/
	MOV		r9, r4
	MOV		r4, r14
	
	MOV		r2, r10
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r6, r7
	barrettPrime	r3, r2, r5
	barrettPrime	r4, r2, r5
	STRH 	r4, [r1, #0]
	
	MOVS	r5, #128
	LSLS	r5, #1
	MOV		r14, r5
	STRH	r3, [r1, r5]
	
	MOV		r4, r8
	MOV		r3, r9
	
	MOV		r2, r10
	radix2ButterflyBACk_MontNewPPinv	r4, r3, r2, r5, r6, r7
	barrettPrime	r3, r2, r5
	barrettPrime	r4, r2, r5
	MOVS	r5, #128
	STRH	r4, [r1, r5]
	
	ADD		r5, r5, r14
	STRH	r3, [r1, r5]
	
	
	ADDS	r0, r0, #1*4
	ADDS	r1, r1, #1*2
	
	POP		{r7}
	ADDS	r7, r7, #1
	CMP		r7, #65
	BNE		NTT_backward3329new2_21level_loop
	
multiply_invN
;	MOVS	r2, #64
;	LSLS	r2, #2
;	SUBS	r0, r0, r2
;	
;	MOVS	r7, #0
;	MOVS	r4, #1
;	LSLS	r4, #8
;	
;;	LDR		r6, =3327
;	LDR		r6, =-26
;multiply_invN_loop
;	LDR 	r3, [r0, #0]
;	
;	MULS	r3, r6, r3
;;;	modPrime r3
;;	LSLS	r2, r3, #24
;;	LSRS	r2, #24
;;	ASRS	r3, r3, #8
;;	MOVS	r5, #13
;;	MULS	r2, r5, r2
;;	SUBS	r3, r2, r3
;;	MEND
;	barrettPrime	r3, r2, r5
;	
;	STR		r3, [r0, #0]
;	ADDS	r0, r0, #1*4
;	ADDS	r7, r7, #1
;	CMP		r7, r4
;	BNE		multiply_invN_loop

;	POP		{r1}
	
	
	POP	{r4}
	MOV	r12, r4
	POP	{r4}
	MOV	r11, r4
	POP	{r4}
	MOV	r10, r4
	POP	{r4}
	MOV	r9, r4
	POP {r4}
	MOV	r8, r4
	POP {r0,r1,r2,r3,r4,r5,r6,r7,pc}
	END
