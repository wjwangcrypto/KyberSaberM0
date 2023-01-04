#include "SABER_params.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "SABER_indcpa.h"
#include "api.h"
#include "verify.h"
#include "rng.h"
#include "fips202.h"


int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  int i;

  indcpa_kem_keypair(pk, sk); // sk[0:SABER_INDCPA_SECRETKEYBYTES-1] <-- sk
	/*change, add*/
//  for (i = 0; i < SABER_INDCPA_PUBLICKEYBYTES; i++)
//    sk[i + SABER_INDCPA_SECRETKEYBYTES] = pk[i]; // sk[SABER_INDCPA_SECRETKEYBYTES:SABER_INDCPA_SECRETKEYBYTES+SABER_INDCPA_SECRETKEYBYTES-1] <-- pk
	memcpy(sk + SABER_INDCPA_SECRETKEYBYTES, pk, SABER_INDCPA_PUBLICKEYBYTES);
	
  sha3_256(sk + SABER_SECRETKEYBYTES - 64, pk, SABER_INDCPA_PUBLICKEYBYTES); // Then hash(pk) is appended.

  randombytes(sk + SABER_SECRETKEYBYTES - SABER_KEYBYTES, SABER_KEYBYTES); // Remaining part of sk contains a pseudo-random number.
                                                                           // This is output when check in crypto_kem_dec() fails.
  return (0);
}

int crypto_kem_enc(unsigned char *c, unsigned char *k, const unsigned char *pk)
{

  unsigned char kr[64]; // Will contain key, coins
  unsigned char buf[64];

  randombytes(buf, 32);
//	//	/*print m*/
//	char bufferm[100];
//	sprintf(bufferm, "\n m = \n");
//	print_str(bufferm);
//	int countm = 0;
//	for(int i = 0; i < 32; i++){
//		sprintf(bufferm, "%x,", buf[i]);
//		print_str(bufferm);
//		countm++;
//		if((countm+1)%32 == 0){
//			sprintf(bufferm, "\n");
//			print_str(bufferm);
//		}
//	}
	
  sha3_256(buf, buf, 32); // BUF[0:31] <-- random message (will be used as the key for client) Note: hash doesnot release system RNG output
//	//	/*print sha3_256(m)*/
//	char buffersham[100];
//	sprintf(buffersham, "\n sha3_256(m) = \n");
//	print_str(buffersham);
//	int countsham = 0;
//	for(int i = 0; i < 32; i++){
//		sprintf(buffersham, "%x,",buf[i]);
//		print_str(buffersham);
//		countsham++;
//		if((countsham+1)%32 == 0){
//			sprintf(buffersham, "\n");
//			print_str(buffersham);
//		}
//	}
	
  sha3_256(buf + 32, pk, SABER_INDCPA_PUBLICKEYBYTES); // BUF[32:63] <-- Hash(public key);  Multitarget countermeasure for coins + contributory KEM
// 	//	/*print buf*/
//	char bufferbuf[100];
//	sprintf(bufferbuf, "\n buf = \n");
//	print_str(bufferbuf);
//	int countbuf = 0;
//	for(int i = 0; i < 64; i++){
//		sprintf(bufferbuf, "%x,",buf[i]);
//		print_str(bufferbuf);
//		countbuf++;
//		if((countbuf+1)%32 == 0){
//			sprintf(bufferbuf, "\n");
//			print_str(bufferbuf);
//		}
//	}
	
  sha3_512(kr, buf, 64);               // kr[0:63] <-- Hash(buf[0:63]);
// 	//	/*print kr*/
//	char bufferkr[100];
//	sprintf(bufferkr, "\n kr = \n");
//	print_str(bufferkr);
//	int countkr = 0;
//	for(int i = 0; i < 64; i++){
//		sprintf(bufferkr, "%x,",kr[i]);
//		print_str(bufferkr);
//		countkr++;
//		if((countkr+1)%32 == 0){
//			sprintf(bufferkr, "\n");
//			print_str(bufferkr);
//		}
//	}                                      // K^ <-- kr[0:31]
                                       // noiseseed (r) <-- kr[32:63];
  indcpa_kem_enc(buf, kr + 32, pk, c); // buf[0:31] contains message; kr[32:63] contains randomness r;

  sha3_256(kr + 32, c, SABER_BYTES_CCA_DEC);

  sha3_256(k, kr, 64); // hash concatenation of pre-k and h(c) to k

  return (0);
}

int crypto_kem_dec(unsigned char *k, const unsigned char *c, const unsigned char *sk)
{
  int i, fail;
  unsigned char cmp[SABER_BYTES_CCA_DEC];
  unsigned char buf[64];
  unsigned char kr[64]; // Will contain key, coins
  const unsigned char *pk = sk + SABER_INDCPA_SECRETKEYBYTES;

  indcpa_kem_dec(sk, c, buf); // buf[0:31] <-- message
//		//	/*print mp*/
//	char buffermp[100];
//	sprintf(buffermp, "\n mp = \n");
//	print_str(buffermp);
//	int countmp = 0;
//	for(int i = 0; i < 32; i++){
//		sprintf(buffermp, "%x,", buf[i]);
//		print_str(buffermp);
//		countmp++;
//		if((countmp+1)%32 == 0){
//			sprintf(buffermp, "\n");
//			print_str(buffermp);
//		}
//	}

  // Multitarget countermeasure for coins + contributory KEM
  for (i = 0; i < 32; i++) // Save hash by storing h(pk) in sk
    buf[32 + i] = sk[SABER_SECRETKEYBYTES - 64 + i];
	
// 	//	/*print buf*/
//	char bufferbuf[100];
//	sprintf(bufferbuf, "\n buf = \n");
//	print_str(bufferbuf);
//	int countbuf = 0;
//	for(int i = 0; i < 64; i++){
//		sprintf(bufferbuf, "%x,",buf[i]);
//		print_str(bufferbuf);
//		countbuf++;
//		if((countbuf+1)%32 == 0){
//			sprintf(bufferbuf, "\n");
//			print_str(bufferbuf);
//		}
//	}
	
  sha3_512(kr, buf, 64);
//	 	//	/*print kr*/
//	char bufferkr[100];
//	sprintf(bufferkr, "\n kr = \n");
//	print_str(bufferkr);
//	int countkr = 0;
//	for(int i = 0; i < 64; i++){
//		sprintf(bufferkr, "%x,",kr[i]);
//		print_str(bufferkr);
//		countkr++;
//		if((countkr+1)%32 == 0){
//			sprintf(bufferkr, "\n");
//			print_str(bufferkr);
//		}
//	}

  indcpa_kem_enc(buf, kr + 32, pk, cmp);

  fail = verify(c, cmp, SABER_BYTES_CCA_DEC);

  sha3_256(kr + 32, c, SABER_BYTES_CCA_DEC); // overwrite coins in kr with h(c)

  cmov(kr, sk + SABER_SECRETKEYBYTES - SABER_KEYBYTES, SABER_KEYBYTES, fail);

  sha3_256(k, kr, 64); // hash concatenation of pre-k and h(c) to k

  return (0);
}
