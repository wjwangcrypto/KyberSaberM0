#include <string.h>
#include <stdint.h>
#include "SABER_indcpa.h"
//#include "poly.h"
#include "pack_unpack.h"
//#include "poly_mul.c"
//#include "poly_mul.h"
#include "rng.h"
#include "fips202.h"
#include "SABER_params.h"

///*print mediate-result out to test*/
//#include "print_out.h"

/*gen poly, need to add this head file*/
#include "cbd.h"
#include "pack_unpack.h"


///*NTT transform, asm files*/
extern void NTT_forward3329new2(uint16_t *, __INT32_TYPE__ *, uint16_t *);
extern void NTT_backward3329new2(__INT32_TYPE__ *, __UINT16_TYPE__ *);
extern void PairMul3329new2(__UINT16_TYPE__ *, __UINT16_TYPE__ *, __UINT16_TYPE__ *);

extern void NTT_forward12289new1(uint16_t *, __INT32_TYPE__ *, uint16_t *);
extern void NTT_backward12289new2(__INT32_TYPE__ *, __UINT16_TYPE__ *);
extern void PairMul12289new2(__UINT16_TYPE__ *, __UINT16_TYPE__ *, __UINT16_TYPE__ *);

extern void Garner332912289new1(__UINT16_TYPE__ *, __UINT16_TYPE__ *);


#define h1 (1 << (SABER_EQ - SABER_EP - 1))
#define h2 ((1 << (SABER_EP - 2)) - (1 << (SABER_EP - SABER_ET - 1)) + (1 << (SABER_EQ - SABER_EP - 1)))


/*gen poly in another way, need this*/
static inline shake128incctx shake128_absorb_seed(const uint8_t seed[SABER_SEEDBYTES]){

    shake128incctx ctx;
    shake128_inc_init(&ctx);
    shake128_inc_absorb(&ctx, seed, SABER_SEEDBYTES);
    shake128_inc_finalize(&ctx);

    return ctx;

}


void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES])
{

	uint8_t seed_A[SABER_SEEDBYTES];
	uint8_t seed_s[SABER_NOISE_SEEDBYTES];

	char bufferseed_A[100];
	char bufferseed_s[100];
	
	randombytes(seed_A, SABER_SEEDBYTES);

	shake128(seed_A, SABER_SEEDBYTES, seed_A, SABER_SEEDBYTES); // for not revealing system RNG state	
	
	randombytes(seed_s, SABER_NOISE_SEEDBYTES);

	MatrixVectorMulKeyPairNTT(pk, sk, seed_A, seed_s);
	
	memcpy(pk + SABER_POLYVECCOMPRESSEDBYTES, seed_A, sizeof(seed_A));
}

void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC])
{	
	int i, j;
	
	const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;

	MatrixVectorMulEncNTT(seed_sp, seed_A, pk, m, ciphertext);
	
}

void indcpa_kem_dec(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES])
{

	InnerProdDecNTT(sk, ciphertext, m);
	
}

#define MAX(a,b) (((a)>(b))?(a):(b))
void MatrixVectorMulKeyPairNTT( uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], uint8_t seed_A[SABER_SEEDBYTES], uint8_t seed_s[SABER_NOISE_SEEDBYTES]){
	
	int i, j;		
	uint8_t buftmp[MAX(SABER_POLYBYTES, SABER_POLYCOINBYTES)];
	uint16_t poly[SABER_N];	
	uint16_t s3329[SABER_N];
	uint16_t s12289[SABER_N];
	uint16_t aNTT[SABER_N];	
	uint16_t b_acc3329[SABER_L][SABER_N] = {0};
	uint16_t b_acc12289[SABER_L][SABER_N] = {0};
	__INT32_TYPE__ tmp[256] = {0};
	
	shake128incctx shake_s_ctx = shake128_absorb_seed(seed_s);
	shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);
	
	char buffers[100];
	for(i = 0; i < SABER_L; i++){
		shake128_inc_squeeze(buftmp, SABER_POLYCOINBYTES, &shake_s_ctx);
		cbd(poly, buftmp);
		
		POLq2BS(sk+i*SABER_POLYBYTES, poly);
		
		/*multiNTT_forward transform of s[i]*/
		NTT_forward3329new2(poly, tmp, s3329);
		NTT_forward12289new1(poly, tmp, s12289);
		for(j = 0; j < SABER_L; j++){
			shake128_inc_squeeze(buftmp, SABER_POLYBYTES, &shake_A_ctx);
			BS2POLq(buftmp, poly);
			
			/*multiNTT_forward3329 transform of A[i][j]*/	
			NTT_forward3329new2(poly, tmp, aNTT);
			/*pair-mul3329 multiply A[i][j] and s[i]*/
			PairMul3329new2(aNTT, s3329, b_acc3329[j]);
			
			/*multiNTT_forward12289 transform of A[i][j]*/
			NTT_forward12289new1(poly, tmp, aNTT);
			/*pair-mul12289 multiply A[i][j] and s[i]*/
			PairMul12289new2(aNTT, s12289, b_acc12289[j]);
		}		
	}
	
		/*add*/
	shake128_inc_ctx_release(&shake_A_ctx);
  shake128_inc_ctx_release(&shake_s_ctx);
	
		/*NTT_backward and other operations*/
	for(j = 0; j < SABER_L; j++){
		NTT_backward3329new2(tmp, b_acc3329[j]);
		NTT_backward12289new2(tmp, b_acc12289[j]);
		Garner332912289new1(b_acc3329[j], b_acc12289[j]);
		
		for(i = 0; i < SABER_N; i++){
			b_acc3329[j][i] = (b_acc3329[j][i] + h1) >> (SABER_EQ - SABER_EP);
		}
		
		POLp2BS(pk + j * (SABER_EP * SABER_N / 8), b_acc3329[j]);
	}
	
}


void MatrixVectorMulEncNTT(const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t m[SABER_KEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC])
{
	int i, j;
//	uint16_t sp[SABER_L][SABER_N];
	uint16_t poly[SABER_N];
	uint16_t spNTT3329[SABER_L][SABER_N] = {0};
	uint16_t spNTT12289[SABER_L][SABER_N] = {0};
	uint16_t aNTT[SABER_N];
	uint16_t polybp3329[SABER_N];
	uint16_t polybp12289[SABER_N];
	uint8_t buftmp[MAX(SABER_POLYBYTES, SABER_POLYCOINBYTES)];
	__INT32_TYPE__ tmp[256] = {0};
	
	
	/*genarate vectorsp and NTT_forward transform of vectorsp*/
	shake128incctx shake_s_ctx = shake128_absorb_seed(seed_sp);	
	for(i = 0; i < SABER_L; i++){
		shake128_inc_squeeze(buftmp, SABER_POLYCOINBYTES, &shake_s_ctx);
		cbd(poly, buftmp);
		
		NTT_forward3329new2(poly, tmp, spNTT3329[i]);
		NTT_forward12289new1(poly, tmp, spNTT12289[i]);
	}
	shake128_inc_ctx_release(&shake_s_ctx);
			
	shake128incctx shake_A_ctx = shake128_absorb_seed(seed_A);
	for(i = 0; i < SABER_L; i++){
		/*refresh polybp*/
//		uint16_t polybp3329[SABER_N] = {0};
//		uint16_t polybp12289[SABER_N] = {0};
		for(j = 0; j < SABER_N; j++){
			polybp3329[j] = 0;
			polybp12289[j] = 0;
		}

		for(j = 0; j < SABER_L; j++){
			/*gen polynomial of matrixA*/
			shake128_inc_squeeze(buftmp, SABER_POLYBYTES, &shake_A_ctx);
			BS2POLq(buftmp, poly);
			
			/*NTT_forward3329 transform of polyA---A[i][j]*/
			NTT_forward3329new2(poly, tmp, aNTT);
			
			/*pairmul3329 multiply of A[i][j]*sp[j]*/
			PairMul3329new2(aNTT, spNTT3329[j], polybp3329);
			
			/*NTT_forward12289 transform of polyA---A[i][j]*/
			NTT_forward12289new1(poly, tmp, aNTT);
			
			/*pairmul12289 multiply of A[i][j]*sp[j]*/
			PairMul12289new2(aNTT, spNTT12289[j], polybp12289);
			
		}
		
		/*NTT_backward and other operations*/
		NTT_backward3329new2(tmp, polybp3329);	
			
		NTT_backward12289new2(tmp, polybp12289);
			
		Garner332912289new1(polybp3329, polybp12289);			
			
		for(j = 0; j < SABER_N; j++){
			polybp3329[j] = (polybp3329[j] + h1) >> (SABER_EQ - SABER_EP);
		}	
			
		POLp2BS(ciphertext + i * (SABER_EP * SABER_N / 8), polybp3329);
	}
	shake128_inc_ctx_release(&shake_A_ctx);
	
	/*gen polynomial of vectorb---b[i]=polyb*/
//	uint16_t polyvp3329[SABER_N]={0};
//	uint16_t polyvp12289[SABER_N]={0};
	for(j = 0; j < SABER_N; j++){
			polybp3329[j] = 0;
			polybp12289[j] = 0;
		}	
	
	for(i = 0; i < SABER_L; i++){
		BS2POLp(pk + i * (SABER_EP * SABER_N / 8), poly);
			
		/*NTT_forward3329 transform of polyb*/
		NTT_forward3329new2(poly, tmp, aNTT);
			
		/*pairmul3329 multiply of b[i]*sp[i]*/
		PairMul3329new2(aNTT, spNTT3329[i], polybp3329);
		
		/*NTT_forward12289 transform of polyb*/
		NTT_forward12289new1(poly, tmp, aNTT);			
			
		/*pairmul12289 multiply of b[i]*sp[i]*/
		PairMul12289new2(aNTT, spNTT12289[i], polybp12289);			
	}
	
	/*NTT_backward and other operations*/
	NTT_backward3329new2(tmp, polybp3329);
	NTT_backward12289new2(tmp, polybp12289);
	Garner332912289new1(polybp3329, polybp12289);
	
//	uint16_t mp[SABER_N];
	BS2POLmsg(m, poly);
	for (j = 0; j < SABER_N; j++)
	{
		polybp3329[j] = (polybp3329[j] - (poly[j] << (SABER_EP - 1)) + h1) >> (SABER_EP - SABER_ET);
	}
			
	POLT2BS(ciphertext + SABER_POLYVECCOMPRESSEDBYTES, polybp3329);

}



void InnerProdDecNTT(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES])
{
	uint16_t poly[SABER_N];
	__INT32_TYPE__ tmp[256] = {0};
//	uint16_t cm[SABER_N];
	uint16_t s3329[SABER_N] = {0};
	uint16_t s12289[SABER_N] = {0};
	uint16_t bNTT[SABER_N] = {0};
//	uint16_t b12289[SABER_N] = {0};
	uint16_t v3329[SABER_N] = {0};
	uint16_t v12289[SABER_N] = {0};
	int i;
	
	/*gen polynomial of vectors and vectorbp, compute v = vectors*vectorbp*/
	for(i = 0; i < SABER_L; i++)
	{
		BS2POLq(sk + i * SABER_POLYBYTES, poly);
			/*add code to transform polys in the range [-u, u]*/
		for(int j = 0; j < SABER_N; j++){
			if(poly[j] > 4096){
				poly[j] = poly[j] - 8192;
			}
		}
		/*NTT_forward of polys---s[i]*/
		NTT_forward3329new2(poly, tmp, s3329);
		NTT_forward12289new1(poly, tmp, s12289);
		
		BS2POLp(ciphertext + i * (SABER_EP * SABER_N / 8), poly);
		/*multiNTT_forward3329 transform of polyb---b[i]*/
		NTT_forward3329new2(poly, tmp, bNTT);
		/*pairmul multiply*/
		PairMul3329new2(s3329, bNTT, v3329);
		
		/*multiNTT_forward12289 transform of polyb---b[i]*/
		NTT_forward12289new1(poly, tmp, bNTT);
		/*pairmul multiply*/
		PairMul12289new2(s12289, bNTT, v12289);	
	}
	/*NTT_backward and other operations*/
	NTT_backward3329new2(tmp, v3329);
	NTT_backward12289new2(tmp, v12289);
	Garner332912289new1(v3329, v12289);	
	
	BS2POLT(ciphertext + SABER_POLYVECCOMPRESSEDBYTES, poly);
	for (i = 0; i < SABER_N; i++)
	{
		v3329[i] = (v3329[i] + h2 - (poly[i] << (SABER_EP - SABER_ET))) >> (SABER_EP - 1);
//		v[i] = ((v[i] + h2 - (cm[i] << (SABER_EP - SABER_ET)))%1024) >> (SABER_EP - 1);
	}	
	
	POLmsg2BS(m, v3329);
}
