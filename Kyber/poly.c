#include <stdint.h>
#include "params.h"
#include "poly.h"
//#include "ntt.h"
#include "reduce.h"
#include "cbd.h"
#include "symmetric.h"

extern void NTT_forward3329new2(int16_t *, __INT32_TYPE__ *, int16_t *);
extern void NTT_backward3329new2(__INT32_TYPE__ *, int16_t *);
extern void PairMul3329new2(int16_t *, int16_t *, int16_t *);

/*************************************************
* Name:        poly_compress
*
* Description: Compression and subsequent serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (of length KYBER_POLYCOMPRESSEDBYTES)
*              - poly *a:    pointer to input polynomial
**************************************************/
void poly_compress(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], poly *a)
{
  unsigned int i,j;
  uint8_t t[8];

  poly_csubq(a);

#if (KYBER_POLYCOMPRESSEDBYTES == 128)
  for(i=0;i<KYBER_N/8;i++) {
    for(j=0;j<8;j++)
      t[j] = ((((uint16_t)a->coeffs[8*i+j] << 4) + KYBER_Q/2)/KYBER_Q) & 15;

    r[0] = t[0] | (t[1] << 4);
    r[1] = t[2] | (t[3] << 4);
    r[2] = t[4] | (t[5] << 4);
    r[3] = t[6] | (t[7] << 4);
    r += 4;
  }
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
  for(i=0;i<KYBER_N/8;i++) {
    for(j=0;j<8;j++)
      t[j] = ((((uint32_t)a->coeffs[8*i+j] << 5) + KYBER_Q/2)/KYBER_Q) & 31;

    r[0] = (t[0] >> 0) | (t[1] << 5);
    r[1] = (t[1] >> 3) | (t[2] << 2) | (t[3] << 7);
    r[2] = (t[3] >> 1) | (t[4] << 4);
    r[3] = (t[4] >> 4) | (t[5] << 1) | (t[6] << 6);
    r[4] = (t[6] >> 2) | (t[7] << 3);
    r += 5;
  }
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

/*************************************************
* Name:        poly_decompress
*
* Description: De-serialization and subsequent decompression of a polynomial;
*              approximate inverse of poly_compress
*
* Arguments:   - poly *r:          pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLYCOMPRESSEDBYTES bytes)
**************************************************/
void poly_decompress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES])
{
  unsigned int i;

#if (KYBER_POLYCOMPRESSEDBYTES == 128)
  for(i=0;i<KYBER_N/2;i++) {
    r->coeffs[2*i+0] = (((uint16_t)(a[0] & 15)*KYBER_Q) + 8) >> 4;
    r->coeffs[2*i+1] = (((uint16_t)(a[0] >> 4)*KYBER_Q) + 8) >> 4;
    a += 1;
  }
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
  unsigned int j;
  uint8_t t[8];
  for(i=0;i<KYBER_N/8;i++) {
    t[0] = (a[0] >> 0);
    t[1] = (a[0] >> 5) | (a[1] << 3);
    t[2] = (a[1] >> 2);
    t[3] = (a[1] >> 7) | (a[2] << 1);
    t[4] = (a[2] >> 4) | (a[3] << 4);
    t[5] = (a[3] >> 1);
    t[6] = (a[3] >> 6) | (a[4] << 2);
    t[7] = (a[4] >> 3);
    a += 5;

    for(j=0;j<8;j++)
      r->coeffs[8*i+j] = ((uint32_t)(t[j] & 31)*KYBER_Q + 16) >> 5;
  }
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
}

/*************************************************
* Name:        poly_tobytes
*
* Description: Serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYBYTES bytes)
*              - poly *a:    pointer to input polynomial
**************************************************/
void poly_tobytes(uint8_t r[KYBER_POLYBYTES], poly *a)
{
  unsigned int i;
  uint16_t t0, t1;

  poly_csubq(a);

  for(i=0;i<KYBER_N/2;i++) {
    t0 = a->coeffs[2*i];
    t1 = a->coeffs[2*i+1];
    r[3*i+0] = (t0 >> 0);
    r[3*i+1] = (t0 >> 8) | (t1 << 4);
    r[3*i+2] = (t1 >> 4);
  }
}

/*************************************************
* Name:        poly_frombytes
*
* Description: De-serialization of a polynomial;
*              inverse of poly_tobytes
*
* Arguments:   - poly *r:          pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of KYBER_POLYBYTES bytes)
**************************************************/
void poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES])
{
  unsigned int i;
  for(i=0;i<KYBER_N/2;i++) {
    r->coeffs[2*i]   = ((a[3*i+0] >> 0) | ((uint16_t)a[3*i+1] << 8)) & 0xFFF;
    r->coeffs[2*i+1] = ((a[3*i+1] >> 4) | ((uint16_t)a[3*i+2] << 4)) & 0xFFF;
  }
}

/*************************************************
* Name:        poly_frommsg
*
* Description: Convert 32-byte message to polynomial
*
* Arguments:   - poly *r:            pointer to output polynomial
*              - const uint8_t *msg: pointer to input message
**************************************************/
void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES])
{
  unsigned int i,j;
  int16_t mask;

#if (KYBER_INDCPA_MSGBYTES != KYBER_N/8)
#error "KYBER_INDCPA_MSGBYTES must be equal to KYBER_N/8 bytes!"
#endif

  for(i=0;i<KYBER_N/8;i++) {
    for(j=0;j<8;j++) {
      mask = -(int16_t)((msg[i] >> j)&1);
      r->coeffs[8*i+j] = mask & ((KYBER_Q+1)/2);
    }
  }
}

/*************************************************
* Name:        poly_tomsg
*
* Description: Convert polynomial to 32-byte message
*
* Arguments:   - uint8_t *msg: pointer to output message
*              - poly *a:      pointer to input polynomial
**************************************************/
void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], poly *a)
{
  unsigned int i,j;
  uint16_t t;

  poly_csubq(a);

  for(i=0;i<KYBER_N/8;i++) {
    msg[i] = 0;
    for(j=0;j<8;j++) {
      t = ((((uint16_t)a->coeffs[8*i+j] << 1) + KYBER_Q/2)/KYBER_Q) & 1;
      msg[i] |= t << j;
    }
  }
}

/*************************************************
* Name:        poly_getnoise_eta1
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA1
*
* Arguments:   - poly *r:             pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce:       one-byte input nonce
**************************************************/
void poly_getnoise_eta1(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[KYBER_ETA1*KYBER_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  cbd_eta1(r, buf);
}

/*************************************************
* Name:        poly_getnoise_eta2
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA2
*
* Arguments:   - poly *r:             pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce:       one-byte input nonce
**************************************************/
void poly_getnoise_eta2(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce)
{
  uint8_t buf[KYBER_ETA2*KYBER_N/4];
  prf(buf, sizeof(buf), seed, nonce);
  cbd_eta2(r, buf);
}


///*************************************************
//* Name:        poly_ntt
//*
//* Description: Computes negacyclic number-theoretic transform (NTT) of
//*              a polynomial in place;
//*              inputs assumed to be in normal order, output in bitreversed order
//*
//* Arguments:   - uint16_t *r: pointer to in/output polynomial
//**************************************************/
//void poly_ntt(poly *r)
//{
//  ntt(r->coeffs);
//  poly_reduce(r);
//}

/*add*/
/*************************************************
* Name:        poly_ntt
*
* Description: Computes negacyclic number-theoretic transform (NTT) of
*              a polynomial in place;
*              inputs assumed to be in normal order, output in bitreversed order
*
* Arguments:   - uint16_t *r: pointer to in/output polynomial
**************************************************/
void poly_ntt(poly *r)
{
	__INT32_TYPE__ tmp[256] = {0};
	NTT_forward3329new2(r->coeffs, tmp, r->coeffs);
}

///*************************************************
//* Name:        poly_invntt_tomont
//*
//* Description: Computes inverse of negacyclic number-theoretic transform (NTT)
//*              of a polynomial in place;
//*              inputs assumed to be in bitreversed order, output in normal order
//*
//* Arguments:   - uint16_t *a: pointer to in/output polynomial
//**************************************************/
//void poly_invntt_tomont(poly *r)
//{
//  invntt(r->coeffs);
//}

/*add*/
/*************************************************
* Name:        poly_invntt
*
* Description: Computes inverse of negacyclic number-theoretic transform (NTT)
*              of a polynomial in place;
*              inputs assumed to be in bitreversed order, output in normal order
*
* Arguments:   - uint16_t *a: pointer to in/output polynomial
**************************************************/
void poly_invntt_tomont(poly *r)
{
	__INT32_TYPE__ tmp[256] = {0};
	NTT_backward3329new2(tmp, r->coeffs);
}

///*************************************************
//* Name:        poly_basemul_montgomery
//*
//* Description: Multiplication of two polynomials in NTT domain
//*
//* Arguments:   - poly *r:       pointer to output polynomial
//*              - const poly *a: pointer to first input polynomial
//*              - const poly *b: pointer to second input polynomial
//**************************************************/
//void poly_basemul_montgomery(poly *r, const poly *a, const poly *b)
//{
//  unsigned int i;
//  for(i=0;i<KYBER_N/4;i++) {
//    basemul(&r->coeffs[4*i], &a->coeffs[4*i], &b->coeffs[4*i], zetas[64+i]);
//    basemul(&r->coeffs[4*i+2], &a->coeffs[4*i+2], &b->coeffs[4*i+2],
//            -zetas[64+i]);
//  }
//}

/*add*/
/*************************************************
* Name:        poly_basemul
*
* Description: Multiplication of two polynomials in NTT domain
*
* Arguments:   - poly *r:       pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_basemulacc(poly *r, poly *a, poly *b)
{
	
	PairMul3329new2(a->coeffs, b->coeffs, r->coeffs);
	
}



///*add*/
///*************************************************
//* Name:        poly_basemulacc_montgomery
//*
//* Description: Multiplication of two polynomials in NTT domain
//*
//* Arguments:   - poly *r:       pointer to output polynomial
//*              - const poly *a: pointer to first input polynomial
//*              - const poly *b: pointer to second input polynomial
//**************************************************/
//void poly_basemulacc_montgomery(poly *c, const poly *a, const poly *b)
//{
//  unsigned int i;
//	poly *r;
//	poly_zeroize(r);
//	
//  for(i=0;i<KYBER_N/4;i++) {
//    basemul(&r->coeffs[4*i], &a->coeffs[4*i], &b->coeffs[4*i], zetas[64+i]);
//    basemul(&r->coeffs[4*i+2], &a->coeffs[4*i+2], &b->coeffs[4*i+2],
//            -zetas[64+i]);
//  }
//	poly_add(c, c, r);
//}


/*************************************************
* Name:        poly_tomont
*
* Description: Inplace conversion of all coefficients of a polynomial
*              from normal domain to Montgomery domain
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void poly_tomont(poly *r)
{
  unsigned int i;
  const int16_t f = (1ULL << 32) % KYBER_Q;
  for(i=0;i<KYBER_N;i++)
    r->coeffs[i] = montgomery_reduce((int32_t)r->coeffs[i]*f);
}

/*************************************************
* Name:        poly_reduce
*
* Description: Applies Barrett reduction to all coefficients of a polynomial
*              for details of the Barrett reduction see comments in reduce.c
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void poly_reduce(poly *r)
{
  unsigned int i;
  for(i=0;i<KYBER_N;i++)
    r->coeffs[i] = barrett_reduce(r->coeffs[i]);
}

/*************************************************
* Name:        poly_csubq
*
* Description: Applies conditional subtraction of q to each coefficient
*              of a polynomial. For details of conditional subtraction
*              of q see comments in reduce.c
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void poly_csubq(poly *r)
{
  unsigned int i;
  for(i=0;i<KYBER_N;i++)
    r->coeffs[i] = csubq(r->coeffs[i]);
}

/*************************************************
* Name:        poly_add
*
* Description: Add two polynomials
*
* Arguments: - poly *r:       pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_add(poly *r, const poly *a, const poly *b)
{
  unsigned int i;
  for(i=0;i<KYBER_N;i++)
    r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/*************************************************
* Name:        poly_sub
*
* Description: Subtract two polynomials
*
* Arguments: - poly *r:       pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void poly_sub(poly *r, const poly *a, const poly *b)
{
  unsigned int i;
  for(i=0;i<KYBER_N;i++)
    r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}


/*add*/
/*************************************************
* Name:        poly_zeroize
*
* Description: Zeros a polynomial
*
* Arguments:   - poly *p: pointer to polynomial
**************************************************/
void poly_zeroize(poly *p) {
  int i;
  for(i = 0; i < KYBER_N; i++)
   p->coeffs[i] = 0;
}

/*add*/
/*************************************************
* Name:        poly_packcompress
*
* Description: Serialization and subsequent compression of a polynomial of a polyvec,
*              writes to a byte string representation of the whole polyvec.
*              Used to compress a polyvec one poly at a time in a loop.
*
* Arguments:   - unsigned char *r:  pointer to output byte string representation of a polyvec (of length KYBER_POLYVECCOMPRESSEDBYTES)
*              - const poly *a:     pointer to input polynomial
*              - int i:             index of to be serialized polynomial in serialized polyec
**************************************************/
void poly_packcompress(unsigned char *r, poly *a, int i) {
    int j, k;

    poly_csubq(a);

#if (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 352))
  uint16_t t[8];

  for(j=0;j<KYBER_N/8;j++) {
    for(k=0;k<8;k++)
      t[k] = ((((uint32_t)a->coeffs[8*j+k] << 11) + KYBER_Q/2) / KYBER_Q) & 0x7ff;

    r[352*i+11*j+ 0] =  t[0] & 0xff;
    r[352*i+11*j+ 1] = (t[0] >>  8) | ((t[1] & 0x1f) << 3);
    r[352*i+11*j+ 2] = (t[1] >>  5) | ((t[2] & 0x03) << 6);
    r[352*i+11*j+ 3] = (t[2] >>  2) & 0xff;
    r[352*i+11*j+ 4] = (t[2] >> 10) | ((t[3] & 0x7f) << 1);
    r[352*i+11*j+ 5] = (t[3] >>  7) | ((t[4] & 0x0f) << 4);
    r[352*i+11*j+ 6] = (t[4] >>  4) | ((t[5] & 0x01) << 7);
    r[352*i+11*j+ 7] = (t[5] >>  1) & 0xff;
    r[352*i+11*j+ 8] = (t[5] >>  9) | ((t[6] & 0x3f) << 2);
    r[352*i+11*j+ 9] = (t[6] >>  6) | ((t[7] & 0x07) << 5);
    r[352*i+11*j+10] = (t[7] >>  3);
  }
#elif (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 320))
    uint16_t t[4];

    for (j = 0; j < KYBER_N / 4; j++) {
        for (k = 0; k < 4; k++)
            t[k] = ((((uint32_t)a->coeffs[4 * j + k] << 10) + KYBER_Q / 2) / KYBER_Q) & 0x3ff;

        r[320*i+5*j+0] =   t[0] & 0xff;
        r[320*i+5*j+1] =  (t[0] >>  8) | ((t[1] & 0x3f) << 2);
        r[320*i+5*j+2] = ((t[1] >>  6) | ((t[2] & 0x0f) << 4)) & 0xff;
        r[320*i+5*j+3] = ((t[2] >>  4) | ((t[3] & 0x03) << 6)) & 0xff;
        r[320*i+5*j+4] =  (t[3] >>  2) & 0xff;
    }
#else
#error "KYBER_POLYVECCOMPRESSEDBYTES needs to in (KYBER_K * {352, 320})"
#endif
}

/*add*/
/*************************************************
* Name:        poly_unpackdecompress
*
* Description: Deserialization and subsequent compression of a polynomial of a polyvec,
*              Used to uncompress a polyvec one poly at a time in a loop.
*
* Arguments:   - const poly *r:     pointer to output polynomial
*              - unsigned char *a:  pointer to input byte string representation of a polyvec (of length KYBER_POLYVECCOMPRESSEDBYTES)
*              - int i:             index of poly in polyvec to decompress
**************************************************/
void poly_unpackdecompress(poly *r, const unsigned char *a, int i) {
  int j;
#if (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 352))
    for(j=0;j<KYBER_N/8;j++)
    {
      r->coeffs[8*j+0] =  (((a[352*i+11*j+ 0]       | (((uint32_t)a[352*i+11*j+ 1] & 0x07) << 8)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+1] = ((((a[352*i+11*j+ 1] >> 3) | (((uint32_t)a[352*i+11*j+ 2] & 0x3f) << 5)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+2] = ((((a[352*i+11*j+ 2] >> 6) | (((uint32_t)a[352*i+11*j+ 3] & 0xff) << 2) | (((uint32_t)a[352*i+11*j+4] & 0x01) << 10)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+3] = ((((a[352*i+11*j+ 4] >> 1) | (((uint32_t)a[352*i+11*j+ 5] & 0x0f) << 7)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+4] = ((((a[352*i+11*j+ 5] >> 4) | (((uint32_t)a[352*i+11*j+ 6] & 0x7f) << 4)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+5] = ((((a[352*i+11*j+ 6] >> 7) | (((uint32_t)a[352*i+11*j+ 7] & 0xff) << 1) | (((uint32_t)a[352*i+11*j+8] & 0x03) <<  9)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+6] = ((((a[352*i+11*j+ 8] >> 2) | (((uint32_t)a[352*i+11*j+ 9] & 0x1f) << 6)) * KYBER_Q) + 1024) >> 11;
      r->coeffs[8*j+7] = ((((a[352*i+11*j+ 9] >> 5) | (((uint32_t)a[352*i+11*j+10] & 0xff) << 3)) * KYBER_Q) + 1024) >> 11;
    }
#elif (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 320))
    for(j=0;j<KYBER_N/4;j++)
    {
      r->coeffs[4*j+0] =  (((a[320*i+5*j+ 0]       | (((uint32_t)a[320*i+5*j+ 1] & 0x03) << 8)) * KYBER_Q) + 512) >> 10;
      r->coeffs[4*j+1] = ((((a[320*i+5*j+ 1] >> 2) | (((uint32_t)a[320*i+5*j+ 2] & 0x0f) << 6)) * KYBER_Q) + 512) >> 10;
      r->coeffs[4*j+2] = ((((a[320*i+5*j+ 2] >> 4) | (((uint32_t)a[320*i+5*j+ 3] & 0x3f) << 4)) * KYBER_Q) + 512) >> 10;
      r->coeffs[4*j+3] = ((((a[320*i+5*j+ 3] >> 6) | (((uint32_t)a[320*i+5*j+ 4] & 0xff) << 2)) * KYBER_Q) + 512) >> 10;
    }
#else
#error "KYBER_POLYVECCOMPRESSEDBYTES needs to be in {320*KYBER_K, 352*KYBER_K}"
#endif
}
