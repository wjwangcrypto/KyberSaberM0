#include "api.h"
//#include "poly.h"
#include "rng.h"
#include "SABER_indcpa.h"
#include "verify.h"
//#include "cpucycles.c"

/*print mediate-result out to test*/
//#include "print_out.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int test_kem_cca()
{


  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];	
  uint8_t ss_a[CRYPTO_BYTES], ss_b[CRYPTO_BYTES];
	
  unsigned char entropy_input[48];
	
  uint64_t i;
   
    	for (i=0; i<48; i++)
        	entropy_input[i] = i;
    	randombytes_init(entropy_input, NULL);
					
	    crypto_kem_keypair(pk, sk);

	    crypto_kem_enc(ct, ss_a, pk);
//				/*print ss_a*/
//			char bufferssa[10];
//			sprintf(bufferssa, "\n ss_a \n");
//			print_str(bufferssa);
//			int countssa = 0;
//			for(int i = 0; i < CRYPTO_BYTES; i++){
//				sprintf(bufferssa, "%x,", ss_a[i]);
//				print_str(bufferssa);
//				countssa++;
//				if((countssa+1)%32 == 0){
//					sprintf(bufferssa, "\n");
//					print_str(bufferssa);
//				}
//			}

	    crypto_kem_dec(ss_b, ct, sk);
//						/*print ss_b*/
//			char bufferssb[10];
//			sprintf(bufferssb, "\n ss_b \n");
//			print_str(bufferssb);
//			int countssb = 0;
//			for(int i = 0; i < CRYPTO_BYTES; i++){
//				sprintf(bufferssb, "%x,", ss_b[i]);
//				print_str(bufferssb);
//				countssb++;
//				if((countssb+1)%32 == 0){
//					sprintf(bufferssb, "\n");
//					print_str(bufferssb);
//				}
//			}
	
  	return 0;
}



int main()
{	
	test_kem_cca();
	return 0;
}
