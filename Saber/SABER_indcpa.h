#ifndef INDCPA_H
#define INDCPA_H

//#include <stdlib.h>
#include "SABER_params.h"
#include <stdint.h>

//void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES]);
//void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC]);
//void indcpa_kem_dec(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES]);

void indcpa_kem_keypair(unsigned char pk[SABER_INDCPA_PUBLICKEYBYTES], unsigned char sk[SABER_INDCPA_SECRETKEYBYTES]);
void indcpa_kem_enc(const unsigned char m[SABER_KEYBYTES], const unsigned char seed_sp[SABER_NOISE_SEEDBYTES], const unsigned char pk[SABER_INDCPA_PUBLICKEYBYTES], unsigned char ciphertext[SABER_BYTES_CCA_DEC]);
void indcpa_kem_dec(const unsigned char sk[SABER_INDCPA_SECRETKEYBYTES], const unsigned char ciphertext[SABER_BYTES_CCA_DEC], unsigned char m[SABER_KEYBYTES]);


/*add*/
void MatrixVectorMulKeyPairNTT( uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], uint8_t seed_A[SABER_SEEDBYTES], uint8_t seed_s[SABER_NOISE_SEEDBYTES]);
void MatrixVectorMulEncNTT(const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t seed_A[SABER_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], const uint8_t m[SABER_KEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC]);
void InnerProdDecNTT(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES]);

#endif