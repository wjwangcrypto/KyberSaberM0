	MACRO
	barrettPrime12289 $opA, $tmp1, $tmp2	
;	barrettPrime r4, r2, r5	
	MOVS	$tmp2, #85
	MOVS	$tmp1, $opA
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, #20
	LDR		$tmp2, =12289
	MULS	$tmp1, $tmp2, $tmp1
	SUBS	$opA, $opA, $tmp1
	MEND
	
	MACRO
	modPrime12289kRED 	$opB, $root12289, $tmp1
	LSLS	$tmp1, $opB, #20
	LSRS	$tmp1, $tmp1, #20
	ASRS	$opB, $opB, #12
	MOVS	$root12289, #3
	MULS	$tmp1, $root12289, $tmp1
	SUBS	$opB, $tmp1, $opB
	MEND
	
	AREA ASM, code, readonly	
	THUMB
	EXPORT Garner332912289new1
		
Garner332912289new1
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
	
	LDR		r2, =40910081
	MOV		r9, r2
	
	LDR		r2, =20455041
	MOV		r10, r2
	
	LDR		r2, =3329
;	LDR		r3, =8077
;	/*(8077*8193=10885 mod 12289)*/
	LDR		r3, =10885
;	MULS	r2, r3, r2
	
	MOVS	r6, #1
	LSLS	r6, r6, #8
	MOV		r8, r6
	
	MOVS	r7, #0
	MOV		r11, r7
garner_loop
;	LDR		r4, [r0, #0]
;	LDR		r5, [r1, #0]
	MOVS	r7, #0
	LDRSH	r4, [r0, r7]
	LDRSH	r5, [r1, r7]
	
	SUBS	r5, r5, r4
	MULS	r5, r3, r5

	modPrime12289kRED 	r5, r6, r7
	barrettPrime12289 	r5, r6, r7
	
	MULS	r5, r2, r5
	ADDS	r4, r4, r5
	
	MOV		r7, r10
	SUBS	r6, r7, r4
	LSRS	r6, r6, #31
	MOV		r5, r9
	MULS	r6, r5, r6
	SUBS	r4, r4, r6
	
	LSLS	r4, r4, #19
	LSRS	r4, r4, #19
	
;	STR		r4, [r0, #0]
	STRH	r4, [r0, #0]

	ADDS	r0, r0, #2
	ADDS	r1, r1, #2
	
	MOV		r7, r11
	ADDS	r7, r7, #1
	MOV		r11, r7
	
	MOV		r6, r8
	MOV		r8, r6
	
	CMP		r7, r6
	BNE		garner_loop
	
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