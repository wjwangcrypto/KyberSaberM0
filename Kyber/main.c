#include "api.h"
#include "poly.h"
#include "rng.h"
#include "indcpa.h"
#include "verify.h"
//#include "cpucycles.c"

/*print mediate-result out to test*/
#include "print_out.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int test_kem_cca()
{


  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key_a[CRYPTO_BYTES] = {0};
	unsigned char key_b[CRYPTO_BYTES] = {0};
	
  unsigned char entropy_input[48];
	
  uint64_t i;

	    for (int i=0; i<48; i++)
        entropy_input[i] = i;

			randombytes_init(entropy_input, NULL, 256);
					
	    crypto_kem_keypair(pk, sk);

			crypto_kem_enc(ct, key_a, pk);
//			/*print ss_a*/
//			char bufferssa[10];
//			sprintf(bufferssa, "\n key_a \n");
//			print_str(bufferssa);
//			int countssa = 0;
//			for(int i = 0; i < CRYPTO_BYTES; i++){
//				sprintf(bufferssa, "%x,", key_a[i]);
//				print_str(bufferssa);
//				countssa++;
//				if((countssa+1)%32 == 0){
//					sprintf(bufferssa, "\n");
//					print_str(bufferssa);
//				}
//			}
			
				crypto_kem_dec(key_b, ct, sk);	 
//			/*print ss_b*/
//			char bufferssb[10];
//			sprintf(bufferssb, "\n key_b \n");
//			print_str(bufferssb);
//			int countssb = 0;
//			for(int i = 0; i < CRYPTO_BYTES; i++){
//				sprintf(bufferssb, "%x,", key_b[i]);
//				print_str(bufferssb);
//				countssb++;
//				if((countssb+1)%32 == 0){
//					sprintf(bufferssb, "\n");
//					print_str(bufferssb);
//				}
//			}			

//	    // Functional verification: check if ss_a == ss_b?
//	    for(i=0; i<SABER_KEYBYTES; i++)
//	    {
//		printf("%u \t %u\n", ss_a[i], ss_b[i]);
//		if(ss_a[i] != ss_b[i])
//		{
//			printf(" ----- ERR CCA KEM ------\n");		
//			break;
//		}
//	    }


  	return 0;
}



int main()
{
	test_kem_cca();
	return 0;
}