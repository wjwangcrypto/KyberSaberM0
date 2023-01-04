	MACRO 
	radix2ButterflyK	$opA, $opB, $root12289, $tmp1, $K
	MULS	$opB, $root12289, $opB
	LSLS	$tmp1, $opB, #20
	LSRS	$tmp1, $tmp1, #20
	ASRS	$opB, $opB, #12
;	MOVS	$root12289, #3
	MULS	$tmp1, $K, $tmp1
	SUBS	$tmp1, $tmp1, $opB
	
	SUBS	$opB, $opA, $tmp1
	ADDS	$opA, $opA, $tmp1
	MEND
	
	MACRO 
	radix2Butterfly_MontPPinv $opA , $opB , $root12289 , $tmp1, $prime, $primeINV
	MULS	$opB, $root12289, $opB
	UXTH	$tmp1, $opB
	ASRS	$opB, $opB, #16
;	LDR		$tmp1, =-12287
;	LTORG
	MULS	$tmp1, $primeINV, $tmp1
	UXTH	$tmp1, $tmp1
;	LDR		$tmp1, =12289
;	LTORG
	MULS	$tmp1, $prime, $tmp1
	ASRS	$tmp1, $tmp1, #16
	SUBS	$tmp1, $opB, $tmp1
	
	SUBS	$opB, $opA, $tmp1
	ADDS	$opA, $opA, $tmp1
	MEND	
	
	MACRO 
	radix2Butterfly_MontNewPPinv $opA , $opB , $root12289 , $tmp1, $prime, $primeINV
	MULS	$opB, $root12289, $opB
	MOVS	$tmp1, $opB
;	LDR		$tmp1, =-12287
;	LTORG
	MULS	$tmp1, $primeINV, $tmp1
	UXTH	$tmp1, $tmp1
;	LDR		$tmp1, =12289
;	LTORG
	MULS	$tmp1, $prime, $tmp1
	SUBS	$tmp1, $opB, $tmp1
	ASRS	$tmp1, $tmp1, #16
	
	SUBS	$opB, $opA, $tmp1
	ADDS	$opA, $opA, $tmp1
	MEND
	
	MACRO
	barrettPrime  $opA, $tmp1, $tmp2
;	MOVS	$tmp2, #85
	LDR		$tmp2, =341
	MOVS	$tmp1, $opA
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, #22
	LDR		$tmp2, =12289
	MULS	$tmp1, $tmp2, $tmp1
	SUBS	$opA, $opA, $tmp1
	MEND


	AREA ASM, code, readonly	
	THUMB
	EXPORT NTT_forward12289new1	
		
	IMPORT root12289_12_8kRED
	IMPORT root12289_34_8kRED		
	IMPORT root12289_56_8Mont
	IMPORT root12289_78_8Mont
		
NTT_forward12289new1
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
	
	PUSH	{r2}
	
NTT_forward12289new1_12level	
	LDR r2, =root12289_12_8kRED
	LTORG
	LDRH	r5, [r2, #0]
	MOV		r10, r5
	LDRH	r5, [r2, #2]
	MOV		r11, r5
	LDRH	r5, [r2, #4]
	MOV		r12, r5
	
	MOVS	r7, #1
	MOVS	r6, #3
NTT_forward12289new1_12level_loop
	;/*level-1*/	
	MOVS	r5, #0
	LDRSH 	r3, [r0, r5]
	
	MOVS	r5, #128
	LSLS	r5, #1
	MOV		r9, r5
	LDRSH	r4, [r0, r5]
	
	MOV		r2, r10
	radix2ButterflyK	r3, r4, r2, r5, r6
	
	MOV		r14, r3
	MOV		r8, r4
	
	MOV		r5, r9
	LSRS	r4, r5, #1
	LDRSH	r3, [r0, r4]
	
	ADD		r5, r5, r4
	LDRSH	r4, [r0, r5]
	radix2ButterflyK	r3, r4, r2, r5, r6

	;/*level-2*/	
	MOV		r9, r4
	MOV		r4, r14
	
	MOV		r2, r11
	radix2ButterflyK	r4, r3, r2, r5, r6
	STR		r4, [r1, #0]
	MOVS	r5, #128
	LSLS	r5, #1
	STR		r3, [r1, r5]

	MOV		r4, r8
	MOV		r3, r9
	
	MOV		r9, r5
	
	MOV		r2, r12
	radix2ButterflyK	r4, r3, r2, r5, r6
	MOV		r5, r9
	LSLS	r5, #1
	STR		r4, [r1, r5]
	
	ADD		r5, r5, r9
	STR		r3, [r1, r5]	
	
	ADDS	r0, r0, #1*2
	ADDS	r1, r1, #1*4
	ADDS	r7, r7, #1
	CMP		r7, #65
	BNE		NTT_forward12289new1_12level_loop	
	
NTT_forward12289new1_34level
	MOVS	r2, #64
	LSLS	r2, #2
	SUBS	r1, r1, r2
	
	LDR 	r2, =root12289_34_8kRED	
	MOVS	r7, #1
NTT_forward12289new1_34level_outerloop
	PUSH	{r7}
	MOVS	r7, #3
	
	LDRH	r0, [r2, #0]
	LDRH	r5, [r2, #2]
	MOV		r11, r5
	LDRH	r5, [r2, #4]
	MOV		r12, r5
	MOV		r10, r2
	
	MOVS	r6, #1
NTT_forward12289new1_34level_innerloop
;	/*level-3*/
	LDR 	r3, [r1, #0]
	
	MOVS	r5, #128
	LDR		r4, [r1, r5]
	
	radix2ButterflyK	r3, r4, r0, r5, r7
	MOV		r14, r3
	MOV		r8, r4
	
	LDR		r5, [r1, #64]
	MOV		r3, r5
	
	MOVS	r5, #128
	ADDS	r5, r5, #64
	LDR		r4, [r1, r5]
	radix2ButterflyK	r3, r4, r0, r5, r7	

	;/*level-4*/	
	MOV		r9, r4
	MOV		r4, r14	
	
	MOV		r2, r11
	radix2ButterflyK	r4, r3, r2, r5, r7	
	STR 	r4, [r1, #0]
	STR		r3, [r1, #64]

	MOV		r4, r8
	MOV		r3, r9
	
	MOV		r2, r12
	radix2ButterflyK	r4, r3, r2, r5, r7	
	MOVS	r5, #128
	STR		r4, [r1, r5]
	
	ADDS	r5, r5, #64
	STR		r3, [r1, r5]

	ADDS	r1, r1, #1*4
	ADDS	r6, r6, #1
	CMP		r6, #17
	BNE		NTT_forward12289new1_34level_innerloop
	
	MOV		r2, r10
	ADDS	r2, r2, #3*2
	
	MOVS	r3, #192
	ADDS	r1, r1, r3
	
	POP		{r7}
	ADDS	r7, r7, #1
	CMP		r7, #5
	BNE		NTT_forward12289new1_34level_outerloop
	
NTT_forward12289new1_56level	
	MOVS	r2, #64
	LSLS	r2, #4
	SUBS	r1, r1, r2
		
	LDR 	r2, =root12289_56_8Mont
	MOVS	r7, #1
	LDR		r0, =12289
NTT_forward12289new1_56level_outerloop
	PUSH	{r7}
	LDR		r7, =-12287
	
	LDRH	r5, [r2, #0]
	MOV		r10, r5
	LDRH	r5, [r2, #2]
	MOV		r11, r5
	LDRH	r5, [r2, #4]
	MOV		r12, r5
	PUSH	{r2}
	
	MOVS	r6, #1
NTT_forward12289new1_56level_innerloop	
;	/*level-5*/
	LDR 	r3, [r1, #0]
	LDR		r4, [r1, #32]
	
	MOV		r2, r10
	radix2Butterfly_MontNewPPinv	r3, r4, r2, r5, r0, r7
	
	MOV		r14, r3
	MOV		r8, r4
	
	LDR		r3, [r1, #16]
	LDR		r4, [r1, #48]
	radix2Butterfly_MontNewPPinv	r3, r4, r2, r5, r0, r7	

;	/*level-6*/		
	MOV		r9, r4
	MOV		r4, r14
	
	MOV		r2, r11
	radix2Butterfly_MontNewPPinv	r4, r3, r2, r5, r0, r7
	STR 	r4, [r1, #0]
	STR		r3, [r1, #16]

	MOV		r4, r8
	MOV		r3, r9
	
	MOV		r2, r12
	radix2Butterfly_MontNewPPinv	r4, r3, r2, r5, r0, r7
	
	STR		r4, [r1, #32]
	STR		r3, [r1, #48]
	
	ADDS	r1, r1, #1*4
	
	ADDS	r6, r6, #1
	CMP		r6, #5
	BNE		NTT_forward12289new1_56level_innerloop
	
	POP		{r2}
	ADDS	r2, r2, #3*2
	
	MOVS	r3, #48
	ADDS	r1, r1, r3
	
	POP		{r7}
	ADDS	r7, r7, #1
	CMP		r7, #17
	BNE		NTT_forward12289new1_56level_outerloop
	
NTT_forward12289new1_78level
	MOVS	r2, #64
	LSLS	r2, #4
	SUBS	r1, r1, r2
		
	LDR 	r0, =root12289_78_8Mont
	LDR		r6, =12289
	MOVS	r7, #1
NTT_forward12289new1_78level_outerloop
	PUSH	{r7}
	LDR		r7, =-12287
	
	LDRH	r2, [r0, #0]
	LDRH	r5, [r0, #2]
	MOV		r11, r5
	LDRH	r5, [r0, #4]
	MOV		r12, r5
	
;	/*level-7*/
	LDR 	r3, [r1, #0]
	LDR		r4, [r1, #2*4]
	radix2Butterfly_MontNewPPinv	r3, r4, r2, r5, r6, r7
	MOV		r14, r3
	MOV		r8, r4
	
	LDR		r3, [r1, #1*4]
	LDR		r4, [r1, #3*4]
	radix2Butterfly_MontNewPPinv	r3, r4, r2, r5, r6, r7

;	/*level-8*/	
	MOV		r9, r4
	MOV		r4, r14
	
	MOV		r2, r11
	radix2Butterfly_MontNewPPinv	r4, r3, r2, r5, r6, r7	
	barrettPrime		r4, r2, r5	
	barrettPrime		r3, r2, r5

	MOV		r14, r4
	MOV		r4, r8
	MOV		r8, r3
	MOV		r3, r9
;	
	MOV		r2, r12
	radix2Butterfly_MontNewPPinv	r4, r3, r2, r5, r6, r7	
	barrettPrime		r4, r2, r5	
	barrettPrime		r3, r2, r5
	
	ADDS	r1, r1, #4*4
	ADDS	r0, r0, #3*2
	
	POP		{r7}
	ADDS	r7, r7, #1
	
	POP		{r2}
	MOV		r5, r14
	STRH 	r5, [r2, #0]
	
	MOV		r5, r8
	STRH	r5, [r2, #1*2]
	
	STRH	r4, [r2, #2*2]
	
	STRH	r3, [r2, #3*2]
	ADDS	r2, r2, #4*2
	PUSH	{r2}
	
	CMP		r7, #65
	BNE		NTT_forward12289new1_78level_outerloop

	POP		{r2}
	
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