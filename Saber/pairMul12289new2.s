	MACRO
	modPrimekRED 	$opB, $root12289, $tmp1
	LSLS	$tmp1, $opB, #20
	LSRS	$tmp1, $tmp1, #20
	ASRS	$opB, $opB, #12
	MOVS	$root12289, #3
	MULS	$tmp1, $root12289, $tmp1
	SUBS	$opB, $tmp1, $opB
	MEND
	
	MACRO
	barrettPrimeP $opA, $tmp1, $tmp2, $prime	
;	MOVS	$tmp2, #85
	MOVS	$tmp1, $opA
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, #20
;	LDR		$tmp2, =12289
	MULS	$tmp1, $prime, $tmp1
	SUBS	$opA, $opA, $tmp1
	MEND

	AREA ASM, code, readonly	
	THUMB
	EXPORT PairMul12289new2	
		
PairMul12289new2
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
	
	MOVS	r7, #0
	MOV		r12, r7
		
	MOVS	r6, #1
	LSLS	r6, r6, #8	
	MOV		r14, r6
	
	MOVS	r5, #85
	LDR		r4, =12289	
PairMul12289new2_loop
	
	MOVS	r6, #0
	LDRSH	r2, [r0, r6]
	LDRSH	r3, [r1, r6]
	
;	barrettPrimeP	r2, r6, r5, r4
;	barrettPrimeP	r3, r6, r5, r4
	
	MULS	r2, r3, r2		
	modPrimekRED	r2, r3, r6
	
;	LDR		r6, =2731
	LDR		r6, =4091
	MULS	r2, r6, r2
	modPrimekRED	r2, r3, r6
	
	POP		{r3}
	MOVS	r6, #0
	LDRSH	r7, [r3, r6]
	ADDS	r2, r2, r7	
	barrettPrimeP	r2, r6, r5, r4	
	STRH	r2, [r3, #0]
	ADDS	r3, r3, #2
	PUSH	{r3}
	
;	barrettPrimeP	r2, r6, r5, r4
;	STRH	r2, [r0, #0]

	ADDS	r0, r0, #2
	ADDS	r1, r1, #2
	
	MOV		r7, r12
	ADDS	r7, r7, #1
	MOV		r12, r7

	MOV		r6, r14
	CMP		r7, r6
	BNE		PairMul12289new2_loop
	
	POP		{r3}

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
		