/*
 * Copyright (c) 1997-2007  The Stanford SRP Authentication Project
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Redistributions in source or binary form must retain an intact copy
 * of this copyright notice.
 */

#include <stdio.h>
#include <sys/types.h>

#include "config.h"

#include "tommath.h"
//#include "mpi.h"
typedef mp_int * BigInteger;
typedef void * BigIntegerCtx;
typedef void * BigIntegerModAccel;

#define MATH_PRIV

#include "t_defines.h"
//#include "t_pwd.h"
#include "srp_aux.h"

/* Math library interface stubs */
#if 0

BigInteger
BigIntegerFromInt(n)
     unsigned int n;
{
#ifdef OPENSSL
  BIGNUM * a = BN_new();
  if(a)
    BN_set_word(a, n);
  return a;
#elif defined(CRYPTOLIB)
  return bigInit(n);
#elif defined(GNU_MP)
  BigInteger rv = (BigInteger) malloc(sizeof(MP_INT));
  if(rv)
    mpz_init_set_ui(rv, n);
  return rv;
#elif defined(GCRYPT)
  BigInteger rv = gcry_mpi_new(32);
  gcry_mpi_set_ui(rv, n);
  return rv;
#elif defined(MPI) || defined(TOMMATH)
  BigInteger rv = (BigInteger) malloc(sizeof(mp_int));
  if(rv) {
    mp_init(rv);
    mp_set_int(rv, n);
  }
  return rv;
#endif
}

BigInteger
BigIntegerFromBytes(bytes, length)
     const unsigned char * bytes;
     int length;
{
#ifdef OPENSSL
  BIGNUM * a = BN_new();
  BN_bin2bn(bytes, length, a);
  return a;
#elif defined(CRYPTOLIB)
  BigInteger rv, t;
  int i, n;

  rv = bigInit(0);
  if(rv == NULL)
    return rv;
  if(length % 4 == 0)
    RSA_bufToBig(bytes, length, rv);
  else {	/* Wouldn't need this if cryptolib behaved better */
    i = length & 0x3;
    if(length > i)
      RSA_bufToBig(bytes + i, length - i, rv);
    for(n = 0; i > 0; --i)
      n = (n << 8) | *bytes++;
    t = bigInit(n);
    bigLeftShift(t, (length & ~0x3) << 3, t);
    bigAdd(rv, t, rv);
    freeBignum(t);
  }
  return rv;
#elif defined(GNU_MP)
  BigInteger rv = (BigInteger) malloc(sizeof(MP_INT));

# ifdef GMP_IMPEXP
  if(rv) {
    mpz_init(rv);
    mpz_import(rv, length, 1, 1, 1, 0, bytes);
  }
# else
  cstr * hexbuf = cstr_new();

  if(hexbuf) {
    if(rv)
      mpz_init_set_str(rv, t_tohexcstr(hexbuf, bytes, length), 16);
    cstr_clear_free(hexbuf);
  }
# endif /* GMP_IMPEXP */

  return rv;
#elif defined(GCRYPT)
  BigInteger rv;
  gcry_mpi_scan(&rv, GCRYMPI_FMT_USG, bytes, length, NULL);
  return rv;
#elif defined(MPI) || defined(TOMMATH)
  BigInteger rv = (BigInteger) malloc(sizeof(mp_int));
  if(rv) {
    mp_init(rv);
    mp_read_unsigned_bin(rv, (unsigned char *)bytes, length);
  }
  return rv;
#endif
}

int
BigIntegerToBytes(src, dest, destlen)
     BigInteger src;
     unsigned char * dest;
     int destlen;
{
#ifdef OPENSSL
  return BN_bn2bin(src, dest);
#elif defined(CRYPTOLIB)
  int i, j;
  cstr * rawbuf;

  trim(src);
  i = bigBytes(src);
  j = (bigBits(src) + 7) / 8;
  if(i == j)
    RSA_bigToBuf(src, i, dest);
  else {	/* Wouldn't need this if cryptolib behaved better */
    rawbuf = cstr_new();
    cstr_set_length(rawbuf, i);
    RSA_bigToBuf(src, i, rawbuf->data);
    memcpy(dest, rawbuf->data + (i-j), j);
    cstr_clear_free(rawbuf);
  }
  return j;
#elif defined(GNU_MP)
  size_t r = 0;
# ifdef GMP_IMPEXP
  mpz_export(dest, &r, 1, 1, 1, 0, src);
# else
  cstr * hexbuf = cstr_new();

  if(hexbuf) {
    cstr_set_length(hexbuf, mpz_sizeinbase(src, 16) + 1);
    mpz_get_str(hexbuf->data, 16, src);
    r = t_fromhex(dest, hexbuf->data);
    cstr_clear_free(hexbuf);
  }
# endif
  return r;
#elif defined(GCRYPT)
  size_t r = 0;
  gcry_mpi_print(GCRYMPI_FMT_USG, dest, destlen, &r, src);
  return r;
#elif defined(MPI) || defined(TOMMATH)
  mp_to_unsigned_bin(src, dest);
  return mp_unsigned_bin_size(src);
#endif
}

BigIntegerResult
BigIntegerToCstr(BigInteger x, cstr * out)
{
  int n = BigIntegerByteLen(x);
  if(cstr_set_length(out, n) < 0)
    return BIG_INTEGER_ERROR;
  if(cstr_set_length(out, BigIntegerToBytes(x, (unsigned char *)(out->data), n)) < 0)
    return BIG_INTEGER_ERROR;
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerToCstrEx(BigInteger x, cstr * out, int len)
{
  int n;
  if(cstr_set_length(out, len) < 0)
    return BIG_INTEGER_ERROR;
  n = BigIntegerToBytes(x, (unsigned char *)(out->data), len);
  if(n < len) {
    memmove(out->data + (len - n), out->data, n);
    memset(out->data, 0, len - n);
  }
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerToHex(src, dest, destlen)
     BigInteger src;
     char * dest;
     int destlen;
{
#ifdef OPENSSL
  strcpy(dest, BN_bn2hex(src));
#elif defined(CRYPTOLIB)
  trim(src);
  bigsprint(src, dest);
#elif defined(GNU_MP)
  mpz_get_str(dest, 16, src);
#elif defined(GCRYPT)
  gcry_mpi_print(GCRYMPI_FMT_HEX, dest, destlen, NULL, src);
#elif defined(MPI) || defined(TOMMATH)
  mp_toradix(src, dest, 16);
#endif
  return BIG_INTEGER_SUCCESS;
}

static char b64table[] =
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./";

BigIntegerResult
BigIntegerToString(src, dest, destlen, radix)
     BigInteger src;
     char * dest;
     int destlen;
     unsigned int radix;
{
  BigInteger t = BigIntegerFromInt(0);
  char * p = dest;
  char c;

  *p++ = b64table[BigIntegerModInt(src, radix, NULL)];
  BigIntegerDivInt(t, src, radix, NULL);
  while(BigIntegerCmpInt(t, 0) > 0) {
    *p++ = b64table[BigIntegerModInt(t, radix, NULL)];
    BigIntegerDivInt(t, t, radix, NULL);
  }
  BigIntegerFree(t);
  *p-- = '\0';
  /* reverse the string */
  while(p > dest) {
    c = *p;
    *p-- =  *dest;
    *dest++ = c;
  }
  return BIG_INTEGER_SUCCESS;
}

int
BigIntegerBitLen(b)
     BigInteger b;
{
#ifdef OPENSSL
  return BN_num_bits(b);
#elif defined(CRYPTOLIB)
  return bigBits(b);
#elif defined(GNU_MP)
  return mpz_sizeinbase(b, 2);
#elif defined(GCRYPT)
  return gcry_mpi_get_nbits(b);
#elif defined(MPI) || defined(TOMMATH)
  return mp_count_bits(b);
#endif
}

int
BigIntegerCmp(c1, c2)
     BigInteger c1, c2;
{
#ifdef OPENSSL
  return BN_cmp(c1, c2);
#elif defined(CRYPTOLIB)
  return bigCompare(c1, c2);
#elif defined(GNU_MP)
  return mpz_cmp(c1, c2);
#elif defined(GCRYPT)
  return gcry_mpi_cmp(c1, c2);
#elif defined(MPI) || defined(TOMMATH)
  return mp_cmp(c1, c2);
#endif
}

int
BigIntegerCmpInt(c1, c2)
     BigInteger c1;
     unsigned int c2;
{
#ifdef OPENSSL
  if(c1->top > 1)
    return 1;
  else if(c1->top < 1)
    return (c2 > 0) ? -1 : 0;
  else {
    if(c1->d[0] > c2)
      return 1;
    else if(c1->d[0] < c2)
      return -1;
    else
      return 0;
  }
#elif defined(CRYPTOLIB)
  BigInteger t;
  int rv;

  t = bigInit(c2);
  rv = bigCompare(c1, t);
  freeBignum(t);
  return rv;
#elif defined(GNU_MP)
  return mpz_cmp_ui(c1, c2);
#elif defined(TOMMATH)
  return mp_cmp_d(c1, c2);
#elif defined(GCRYPT)
  return gcry_mpi_cmp_ui(c1, c2);
#elif defined(MPI)
  return mp_cmp_int(c1, c2);
#endif
}

BigIntegerResult
BigIntegerLShift(result, x, bits)
     BigInteger result, x;
     unsigned int bits;
{
#ifdef OPENSSL
  BN_lshift(result, x, bits);
#elif defined(CRYPTOLIB)
  bigLeftShift(x, bits, result);
#elif defined(GNU_MP)
  mpz_mul_2exp(result, x, bits);
#elif defined(GCRYPT)
  gcry_mpi_mul_2exp(result, x, bits);
#elif defined(MPI) || defined(TOMMATH)
  mp_mul_2d(x, bits, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerAdd(result, a1, a2)
     BigInteger result, a1, a2;
{
#ifdef OPENSSL
  BN_add(result, a1, a2);
#elif defined(CRYPTOLIB)
  bigAdd(a1, a2, result);
#elif defined(GNU_MP)
  mpz_add(result, a1, a2);
#elif defined(GCRYPT)
  gcry_mpi_add(result, a1, a2);
#elif defined(MPI) || defined(TOMMATH)
  mp_add(a1, a2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerAddInt(result, a1, a2)
     BigInteger result, a1;
     unsigned int a2;
{
#ifdef OPENSSL
  if(result != a1)
    BN_copy(result, a1);
  BN_add_word(result, a2);
#elif defined(CRYPTOLIB)
  BigInteger t;

  t = bigInit(a2);
  bigAdd(a1, t, result);
  freeBignum(t);
#elif defined(GNU_MP)
  mpz_add_ui(result, a1, a2);
#elif defined(GCRYPT)
  gcry_mpi_add_ui(result, a1, a2);
#elif defined(MPI) || defined(TOMMATH)
  mp_add_d(a1, a2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerSub(result, s1, s2)
     BigInteger result, s1, s2;
{
#ifdef OPENSSL
  BN_sub(result, s1, s2);
#elif defined(CRYPTOLIB)
  bigSubtract(s1, s2, result);
#elif defined(GNU_MP)
  mpz_sub(result, s1, s2);
#elif defined(GCRYPT)
  gcry_mpi_sub(result, s1, s2);
#elif defined(MPI) || defined(TOMMATH)
  mp_sub(s1, s2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerSubInt(result, s1, s2)
     BigInteger result, s1;
     unsigned int s2;
{
#ifdef OPENSSL
  if(result != s1)
    BN_copy(result, s1);
  BN_sub_word(result, s2);
#elif defined(CRYPTOLIB)
  BigInteger t;

  t = bigInit(s2);
  bigSubtract(s1, t, result);
  freeBignum(t);
#elif defined(GNU_MP)
  mpz_sub_ui(result, s1, s2);
#elif defined(GCRYPT)
  gcry_mpi_sub_ui(result, s1, s2);
#elif defined(MPI) || defined(TOMMATH)
  mp_sub_d(s1, s2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerMul(result, m1, m2, c)
     BigInteger result, m1, m2;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  BN_CTX * ctx = NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  BN_mul(result, m1, m2, c);
  if(ctx)
    BN_CTX_free(ctx);
#elif defined(CRYPTOLIB)
  bigMultiply(m1, m2, result);
#elif defined(GNU_MP)
  mpz_mul(result, m1, m2);
#elif defined(GCRYPT)
  gcry_mpi_mul(result, m1, m2);
#elif defined(MPI) || defined(TOMMATH)
  mp_mul(m1, m2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerMulInt(result, m1, m2, c)
     BigInteger result, m1;
     unsigned int m2;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  if(result != m1)
    BN_copy(result, m1);
  BN_mul_word(result, m2);
#elif defined(CRYPTOLIB)
  BigInteger t;

  t = bigInit(m2);
  bigMultiply(m1, t, result);
  freeBignum(t);
#elif defined(GNU_MP)
  mpz_mul_ui(result, m1, m2);
#elif defined(GCRYPT)
  gcry_mpi_mul_ui(result, m1, m2);
#elif defined(MPI) || defined(TOMMATH)
  mp_mul_d(m1, m2, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerDivInt(result, d, m, c)
     BigInteger result, d;
     unsigned int m;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  if(result != d)
    BN_copy(result, d);
  BN_div_word(result, m);
#elif defined(CRYPTOLIB)
  BigInteger t, u, q;

  t = bigInit(m);
  u = bigInit(0);
  /* We use a separate variable q because cryptolib breaks if result == d */
  q = bigInit(0);
  bigDivide(d, t, q, u);
  freeBignum(t);
  freeBignum(u);
  bigCopy(q, result);
  freeBignum(q);
#elif defined(GNU_MP)
# ifdef GMP2
  mpz_fdiv_q_ui(result, d, m);
# else
  mpz_div_ui(result, d, m);
# endif
#elif defined(GCRYPT)
  BigInteger t = BigIntegerFromInt(m);
  gcry_mpi_div(result, NULL, d, t, -1);
  BigIntegerFree(t);
#elif defined(MPI) || defined(TOMMATH)
  mp_div_d(d, m, result, NULL);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerMod(result, d, m, c)
     BigInteger result, d, m;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  BN_CTX * ctx = NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  BN_mod(result, d, m, c);
  if(ctx)
    BN_CTX_free(ctx);
#elif defined(CRYPTOLIB)
  bigMod(d, m, result);
#elif defined(GNU_MP)
  mpz_mod(result, d, m);
#elif defined(GCRYPT)
  gcry_mpi_mod(result, d, m);
#elif defined(MPI) || defined(TOMMATH)
  mp_mod(d, m, result);
#endif
  return BIG_INTEGER_SUCCESS;
}

unsigned int
BigIntegerModInt(d, m, c)
     BigInteger d;
     unsigned int m;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  return BN_mod_word(d, m);
#elif defined(CRYPTOLIB)
  BigInteger t, u;
  unsigned char r[4];

  t = bigInit(m);
  u = bigInit(0);
  bigMod(d, t, u);
  bigToBuf(u, sizeof(r), r);
  freeBignum(t);
  freeBignum(u);
  return r[0] | (r[1] << 8) | (r[2] << 16) | (r[3] << 24);
#elif defined(GNU_MP)
  MP_INT result;
  unsigned int i;

  mpz_init(&result);

/* Define GMP2 if you're using an old gmp.h but want to link against a
 * newer libgmp.a (e.g. 2.0 or later). */

# ifdef GMP2
  mpz_fdiv_r_ui(&result, d, m);
# else
  mpz_mod_ui(&result, d, m);
# endif
  i = mpz_get_ui(&result);
  mpz_clear(&result);
  return i;
#elif defined(GCRYPT)
  /* TODO: any way to clean this up??? */
  unsigned char r[4];
  size_t len, i;
  unsigned int ret = 0;
  BigInteger t = BigIntegerFromInt(m);
  BigInteger a = BigIntegerFromInt(0);
  gcry_mpi_mod(a, d, t);
  gcry_mpi_print(GCRYMPI_FMT_USG, r, 4, &len, a);
  for(i = 0; i < len; ++i)
    ret = (ret << 8) | r[i];
  BigIntegerFree(t);
  BigIntegerFree(a);
  return ret;
#elif defined(MPI) || defined(TOMMATH)
  mp_digit r;
  mp_mod_d(d, m, &r);
  return r;
#endif
}

BigIntegerResult
BigIntegerModMul(r, m1, m2, modulus, c)
     BigInteger r, m1, m2, modulus;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  BN_CTX * ctx = NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  BN_mod_mul(r, m1, m2, modulus, c);
  if(ctx)
    BN_CTX_free(ctx);
#elif defined(CRYPTOLIB)
  bigMultiply(m1, m2, r);
  bigMod(r, modulus, r);
#elif defined(GNU_MP)
  mpz_mul(r, m1, m2);
  mpz_mod(r, r, modulus);
#elif defined(GCRYPT)
  gcry_mpi_mulm(r, m1, m2, modulus);
#elif defined(MPI) || defined(TOMMATH)
  mp_mulmod(m1, m2, modulus, r);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerModExp(r, b, e, m, c, a)
     BigInteger r, b, e, m;
     BigIntegerCtx c;
     BigIntegerModAccel a;
{
#ifdef OPENSSL
  BN_CTX * ctx = NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  if(default_modexp) {
    (*default_modexp)(r, b, e, m, c, a);
  }
  else if(a == NULL) {
    BN_mod_exp(r, b, e, m, c);
  }
#if OPENSSL_VERSION_NUMBER >= 0x00906000
  else if(b->top == 1) {  /* 0.9.6 and above has mont_word optimization */
    BN_ULONG B = b->d[0];
    BN_mod_exp_mont_word(r, B, e, m, c, a);
  }
#endif
  else
    BN_mod_exp_mont(r, b, e, m, c, a);
  if(ctx)
    BN_CTX_free(ctx);
#elif defined(CRYPTOLIB)
  bigPow(b, e, m, r);
#elif defined(GNU_MP)
  mpz_powm(r, b, e, m);
#elif defined(GCRYPT)
  gcry_mpi_powm(r, b, e, m);
#elif defined(MPI) || defined(TOMMATH)
  mp_exptmod(b, e, m, r);
#endif
  return BIG_INTEGER_SUCCESS;
}

int
BigIntegerCheckPrime(n, c)
     BigInteger n;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  int rv;
  BN_CTX * ctx = NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  rv = BN_is_prime(n, 25, NULL, c, NULL);
  if(ctx)
    BN_CTX_free(ctx);
  return rv;
#elif defined(CRYPTOLIB)
#if 0
  /*
   * Ugh.  Not only is cryptolib's bigDivide sensitive to inputs
   * and outputs being the same, but now the primeTest needs random
   * numbers, which it gets by calling cryptolib's broken truerand
   * implementation(!)  We have to fake it out by doing our own
   * seeding explicitly.
   */
  static int seeded = 0;
  static unsigned char seedbuf[64];
  if(!seeded) {
    t_random(seedbuf, sizeof(seedbuf));
    seedDesRandom(seedbuf, sizeof(seedbuf));
    memset(seedbuf, 0, sizeof(seedbuf));
    seeded = 1;
  }
#endif /* 0 */
  t_random(NULL, 0);
  return primeTest(n);
#elif defined(GNU_MP)
  return mpz_probab_prime_p(n, 25);
#elif defined(GCRYPT)
  return (gcry_prime_check(n, 0) == GPG_ERR_NO_ERROR);
#elif defined(TOMMATH)
  int rv;
  mp_prime_is_prime(n, 25, &rv);
  return rv;
#elif defined(MPI)
  return (mpp_pprime(n, 25) == MP_YES);
#endif
}

BigIntegerResult
BigIntegerFree(b)
     BigInteger b;
{
#ifdef OPENSSL
  BN_free(b);
#elif defined(CRYPTOLIB)
  freeBignum(b);
#elif defined(GNU_MP)
  mpz_clear(b);
  free(b);
#elif defined(GCRYPT)
  gcry_mpi_release(b);
#elif defined(MPI) || defined(TOMMATH)
  mp_clear(b);
  free(b);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerClearFree(b)
     BigInteger b;
{
#ifdef OPENSSL
  BN_clear_free(b);
#elif defined(CRYPTOLIB)
  /* TODO */
  freeBignum(b);
#elif defined(GNU_MP)
  /* TODO */
  mpz_clear(b);
  free(b);
#elif defined(GCRYPT)
  /* TODO */
  gcry_mpi_release(b);
#elif defined(MPI) || defined(TOMMATH)
  /* TODO */
  mp_clear(b);
  free(b);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerCtx
BigIntegerCtxNew()
{
#ifdef OPENSSL
  return BN_CTX_new();
#else
  return NULL;
#endif
}

BigIntegerResult
BigIntegerCtxFree(ctx)
     BigIntegerCtx ctx;
{
#ifdef OPENSSL
  if(ctx)
    BN_CTX_free(ctx);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerModAccel
BigIntegerModAccelNew(m, c)
     BigInteger m;
     BigIntegerCtx c;
{
#ifdef OPENSSL
  BN_CTX * ctx = NULL;
  BN_MONT_CTX * mctx;
  if(default_modexp)
    return NULL;
  if(c == NULL)
    c = ctx = BN_CTX_new();
  mctx = BN_MONT_CTX_new();
  BN_MONT_CTX_set(mctx, m, c);
  if(ctx)
    BN_CTX_free(ctx);
  return mctx;
#else
  return NULL;
#endif
}

BigIntegerResult
BigIntegerModAccelFree(accel)
     BigIntegerModAccel accel;
{
#ifdef OPENSSL
  if(accel)
    BN_MONT_CTX_free(accel);
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerInitialize()
{
#if OPENSSL_VERSION_NUMBER >= 0x00907000
  ENGINE_load_builtin_engines();
#endif
  return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerFinalize()
{
  return BigIntegerReleaseEngine();
}

BigIntegerResult
BigIntegerUseEngine(const char * engine)
{
#if defined(OPENSSL) && defined(OPENSSL_ENGINE)
  ENGINE * e = ENGINE_by_id(engine);
  if(e) {
    if(ENGINE_init(e) > 0) {
#if OPENSSL_VERSION_NUMBER >= 0x00907000
      /* 0.9.7 loses the BN_mod_exp method.  Pity. */
      const RSA_METHOD * rsa = ENGINE_get_RSA(e);
      if(rsa)
	default_modexp = (modexp_meth)rsa->bn_mod_exp;
#else
      default_modexp = (modexp_meth)ENGINE_get_BN_mod_exp(e);
#endif
      BigIntegerReleaseEngine();
      default_engine = e;
      return BIG_INTEGER_SUCCESS;
    }
    else
      ENGINE_free(e);
  }
#endif
  return BIG_INTEGER_ERROR;
}

BigIntegerResult
BigIntegerReleaseEngine()
{
#if defined(OPENSSL) && defined(OPENSSL_ENGINE)
  if(default_engine) {
    ENGINE_finish(default_engine);
    ENGINE_free(default_engine);
    default_engine = NULL;
    default_modexp = NULL;
  }
#endif
  return BIG_INTEGER_SUCCESS;
}

#endif


BigIntegerResult
BigIntegerFree(BigInteger b)
{
	mp_clear(b);
	free(b);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerClearFree(BigInteger b)
{
	mp_clear(b);
	free(b);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerCtx
BigIntegerCtxNew()
{
	return NULL;
}

BigIntegerResult
BigIntegerCtxFree(BigIntegerCtx ctx)
{
	return BIG_INTEGER_SUCCESS;
}

BigIntegerModAccel
BigIntegerModAccelNew(BigInteger m, BigIntegerCtx c)
{
	return NULL;
}

BigIntegerResult
BigIntegerModAccelFree(BigIntegerModAccel accel)
{
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerInitialize()
{
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerFinalize()
{
	return BigIntegerReleaseEngine();
}

BigIntegerResult
BigIntegerReleaseEngine()
{
	return BIG_INTEGER_SUCCESS;
}

BigInteger
BigIntegerFromInt(unsigned int n)
{
	BigInteger rv = (BigInteger)malloc(sizeof(mp_int));
	if (rv) {
		mp_init(rv);
		mp_set_int(rv, n);
	}
	return rv;
}

BigInteger
BigIntegerFromBytes(const unsigned char * bytes, int length)
{
	BigInteger rv = (BigInteger)malloc(sizeof(mp_int));
	if (rv) {
		mp_init(rv);
		mp_read_unsigned_bin(rv, bytes, length);
	}
	return rv;
}

BigIntegerResult
BigIntegerToCstr(BigInteger x, cstr * out)
{
	int n = BigIntegerByteLen(x);
	if (cstr_set_length(out, n) < 0)
		return BIG_INTEGER_ERROR;
	if (cstr_set_length(out, BigIntegerToBytes(x, out->data, n)) < 0)
		return BIG_INTEGER_ERROR;
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerToCstrEx(BigInteger x, cstr * out, int len)
{
	int n;
	if (cstr_set_length(out, len) < 0)
		return BIG_INTEGER_ERROR;
	n = BigIntegerToBytes(x, (unsigned char *)(out->data), len);
	if (n < len) {
		memmove(out->data + (len - n), out->data, n);
		memset(out->data, 0, len - n);
	}
	return BIG_INTEGER_SUCCESS;
}

int
BigIntegerToBytes(BigInteger src, unsigned char * dest, int destlen)
{
	mp_to_unsigned_bin(src, dest);
	return mp_unsigned_bin_size(src);
}

int
BigIntegerBitLen(BigInteger b)
{
	return mp_count_bits(b);
}

int
BigIntegerCmp(BigInteger c1, BigInteger c2)
{
	return mp_cmp(c1, c2);
}

int
BigIntegerCmpInt(BigInteger c1, unsigned int c2)
{
//	return mp_cmp_int(c1, c2);
	return mp_cmp_d(c1, c2);
}

BigIntegerResult
BigIntegerAdd(BigInteger result, BigInteger a1, BigInteger a2)
{
	mp_add(a1, a2, result);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerAddInt(BigInteger result, BigInteger a1, unsigned int a2)
{
	mp_add_d(a1, a2, result);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerSub(BigInteger result, BigInteger s1, BigInteger s2)
{
	mp_sub(s1, s2, result);
	return BIG_INTEGER_SUCCESS;
}


BigIntegerResult
BigIntegerMul(BigInteger result, BigInteger m1, BigInteger m2, BigIntegerCtx c)
{
	mp_mul(m1, m2, result);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerMod(BigInteger result, BigInteger d, BigInteger m, BigIntegerCtx c)
{
	mp_mod(d, m, result);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerModMul(BigInteger r, BigInteger m1, BigInteger m2, BigInteger modulus, BigIntegerCtx c)
{
	mp_mulmod(m1, m2, modulus, r);
	return BIG_INTEGER_SUCCESS;
}

BigIntegerResult
BigIntegerModExp(BigInteger r, BigInteger b, BigInteger e, BigInteger m, BigIntegerCtx c, BigIntegerModAccel a)
{
	mp_exptmod(b, e, m, r);
	return BIG_INTEGER_SUCCESS;
}

unsigned int
BigIntegerModInt(BigInteger d, unsigned int m, BigIntegerCtx c)
{
	mp_digit r = 0;
	mp_mod_d(d, m, &r);
	return r;
}

BigIntegerResult
BigIntegerDivInt(BigInteger result, BigInteger d, unsigned int m, BigIntegerCtx c)
{
	mp_div_d(d, m, result, NULL);
	return BIG_INTEGER_SUCCESS;
}

static unsigned char b64table[] =
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./";

BigIntegerResult
BigIntegerToString(BigInteger src, char * dest, int destlen, unsigned int radix)
{
	BigInteger t = BigIntegerFromInt(0);
	char * p = dest;
	char c;

	*p++ = b64table[BigIntegerModInt(src, radix, NULL)];
	BigIntegerDivInt(t, src, radix, NULL);
	while (BigIntegerCmpInt(t, 0) > 0) {
		*p++ = b64table[BigIntegerModInt(t, radix, NULL)];
		BigIntegerDivInt(t, t, radix, NULL);
	}
	BigIntegerFree(t);
	*p-- = '\0';
	/* reverse the string */
	while (p > dest) {
		c = *p;
		*p-- = *dest;
		*dest++ = c;
	}
	return BIG_INTEGER_SUCCESS;
}

