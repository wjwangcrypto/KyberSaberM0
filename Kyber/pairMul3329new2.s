	MACRO
	modPrimekREDK $opA, $tmp1, $tmp2
	UXTB	$tmp1, $opA
	ASRS	$opA,  #8
;	MOVS	$tmp2, #13
	MULS	$tmp1, $tmp2, $tmp1
	SUBS	$opA, $tmp1, $opA 
	MEND
	
	MACRO
	barrettPrimeP $opA, $tmp1, $tmp2, $prime
;	MOVS	$tmp2, #315
	MOVS	$tmp1, $opA
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, #20
;	LDR		$tmp2, =3329
	MULS	$tmp1, $prime, $tmp1
	SUBS	$opA, $opA, $tmp1
	MEND
	
	MACRO 
	montPrimeMont  $opA, $tmp1, $tmp2
	UXTH	$tmp1, $opA
	ASRS	$opA, $opA, #16
	LDR		$tmp2, =-3327
	LTORG
	MULS	$tmp1, $tmp2, $tmp1
	UXTH	$tmp1, $tmp1
	LDR		$tmp2, =3329
	LTORG
	MULS	$tmp1, $tmp2, $tmp1
	ASRS	$tmp1, $tmp1, #16
	SUBS	$opA, $opA, $tmp1
	MEND
	
	
	AREA ASM, code, readonly	
	THUMB
	EXPORT PairMul3329new2

	IMPORT root3329_pairmul_7_kRED

PairMul3329new2
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
	
;	PUSH	{r2}
	
	LDR		r7, =root3329_pairmul_7_kRED
	PUSH	{r7}
	LDR		r7, = 2285
;	2285=mod(3073*3073, 3329)
;	LDR		r7, =512	
;//this is inv_2 in backward_NTT (512=mod(3303*3073*3073, 3329); 2088=mod(3303*3073*3073*3073, 3329))
;	LDR		r7, =2088	
;//2088=mod(3303*3073*3073*3073, 3329));
;	LDR		r7, =3327 	
;	//3327=mod(3303*3073, 3329);
;	LDR		r7, =2088
;	2088=mod(3303*3073*2^16, 3329)

	MOV		r9, r7
	
	MOVS	r7, #0
	MOV		r14, r7
PairMul3329new2KREDKa_loop
	MOVS	r7, #0
	LDRSH	r3, [r0, r7]
	LDRSH	r5, [r1, r7]
	ADDS	r7, r7, #2
	LDRSH	r4, [r0, r7]
	LDRSH	r6, [r1, r7]
	ADDS	r1, r1, #4
	
	MOV		r12, r0
	MOV		r11, r1
	
	LDR		r7, =3329
	
	MOV		r0, r3
	MOV		r8, r6
	MULS	r3, r5, r3
	MULS	r6, r4, r6
	
	ADD		r4, r0
	ADD		r5, r8
	MULS	r5, r4, r5
	SUBS	r5, r5, r3
	SUBS	r5, r5, r6
	MOVS	r0, #13
	modPrimekREDK	r5, r1, r0
;	montPrimeMont  r5, r1, r0
	
	modPrimekREDK 	r6, r1, r0
	POP		{r7}
	LDRH	r4, [r7, #0]
	ADDS	r7, r7, #2
	PUSH	{r7}
	MULS	r6, r4, r6
	ADD		r3, r3, r6
	modPrimekREDK r3, r1, r0
;	montPrimeMont  r3, r1, r0
	
	MOV		r4, r9
	MULS	r5, r4, r5
	modPrimekREDK r5, r1, r0
	MULS	r3, r4, r3
	modPrimekREDK r3, r1, r0
;//can or cannot??___make the inv-2 and pair_mul_roots merged, then this operations(mul, modprimekRED) can be saved.
	
;	MOVS	r0, #157
;	LDR		r7, =3329
;	barrettPrimeP	r3, r1, r0, r7
;	barrettPrimeP	r5, r1, r0, r7
	
;	POP		{r2}
	MOVS	r0, #0
	LDRSH	r4, [r2, r0]
	ADDS	r3, r3, r4
	MOVS	r0, #2
	LDRSH	r4, [r2, r0]	
	ADDS	r5, r5, r4

	
	LDR		r0, =315
	LDR		r7, =3329
	barrettPrimeP	r3, r1, r0, r7
	barrettPrimeP	r5, r1, r0, r7
	
	
	MOV		r0, r12
	MOV		r1, r11
;	STR		r3, [r0, #0]
;	STR		r5, [r0, #4]	
;	ADDS	r0, r0, #8
;	STRH	r3, [r0, #0]
;	STRH	r5, [r0, #2]
	STRH	r3, [r2, #0]
	STRH	r5, [r2, #2]
	ADDS	r2, r2, #4
;	PUSH	{r2}
	ADDS	r0, r0, #4
	
;	ADDS	r2, r2, #2
	
	MOV		r7, r14
	ADDS	r7, r7, #1
	MOV		r14, r7
	CMP		r7, #128
	BNE	PairMul3329new2KREDKa_loop
	
;	POP		{r2}
	POP		{r7}
	
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