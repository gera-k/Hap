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

#include "t_defines.h"
//#include "t_pwd.h"
#include "srp.h"

static int library_initialized = 0;

_TYPE( SRP_RESULT )
SRP_initialize_library()
{
  if(library_initialized == 0) {
    BigIntegerInitialize();
    t_stronginitrand();
    library_initialized = 1;
  }
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_finalize_library()
{
  if(library_initialized > 0) {
    library_initialized = 0;
    BigIntegerFinalize();
  }
  return SRP_SUCCESS;
}

static int srp_modulus_min_bits = SRP_DEFAULT_MIN_BITS;

_TYPE( SRP_RESULT )
SRP_set_modulus_min_bits(int minbits)
{
  srp_modulus_min_bits = minbits;
  return SRP_SUCCESS;
}

_TYPE( int )
SRP_get_modulus_min_bits()
{
  return srp_modulus_min_bits;
}

static int
default_secret_bits_cb(int modsize)
{
  return 256;
  /*return modsize;*/   /* Warning: Very Slow */
}

static SRP_SECRET_BITS_CB srp_sb_cb = default_secret_bits_cb;

_TYPE( SRP_RESULT )
SRP_set_secret_bits_cb(SRP_SECRET_BITS_CB cb)
{
  srp_sb_cb = cb;
  return SRP_SUCCESS;
}

_TYPE( int )
SRP_get_secret_bits(int modsize)
{
  return (*srp_sb_cb)(modsize);
}

_TYPE( SRP_SERVER_LOOKUP * )
SRP_SERVER_LOOKUP_new(SRP_SERVER_LOOKUP_METHOD * meth)
{
  SRP_SERVER_LOOKUP * slu = (SRP_SERVER_LOOKUP *) malloc(sizeof(SRP_SERVER_LOOKUP));
  if(slu == NULL)
    return NULL;

  slu->meth = meth;
  slu->data = NULL;
  if(slu->meth->init == NULL || (*slu->meth->init)(slu) == SRP_SUCCESS)
    return slu;
  free(slu);
  return NULL;
}

_TYPE( SRP_RESULT )
SRP_SERVER_LOOKUP_free(SRP_SERVER_LOOKUP * slu)
{
  if(slu->meth->finish)
    (*slu->meth->finish)(slu);
  free(slu);
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_SERVER_do_lookup(SRP_SERVER_LOOKUP * slu, SRP * srp, cstr * username)
{
  return (*slu->meth->lookup)(slu, srp, username);
}

_TYPE( SRP * )
SRP_new(SRP_METHOD * meth)
{
  SRP * srp = (SRP *) malloc(sizeof(SRP));

  if(srp == NULL)
    return NULL;

  srp->flags = 0;
  srp->username = cstr_new();
  srp->bctx = BigIntegerCtxNew();
  srp->modulus = NULL;
  srp->accel = NULL;
  srp->generator = NULL;
  srp->salt = NULL;
  srp->verifier = NULL;
  srp->password = NULL;
  srp->pubkey = NULL;
  srp->secret = NULL;
  srp->u = NULL;
  srp->key = NULL;
  srp->ex_data = cstr_new();
  srp->param_cb = NULL;
  srp->meth = meth;
  srp->meth_data = NULL;
  srp->slu = NULL;
  if(srp->meth->init == NULL || (*srp->meth->init)(srp) == SRP_SUCCESS)
    return srp;
  free(srp);
  return NULL;
}

_TYPE( SRP_RESULT )
SRP_free(SRP * srp)
{
  if(srp->meth->finish)
    (*srp->meth->finish)(srp);

  if(srp->username)
    cstr_clear_free(srp->username);
  if(srp->modulus)
    BigIntegerFree(srp->modulus);
  if(srp->accel)
    BigIntegerModAccelFree(srp->accel);
  if(srp->generator)
    BigIntegerFree(srp->generator);
  if(srp->salt)
    cstr_clear_free(srp->salt);
  if(srp->verifier)
    BigIntegerClearFree(srp->verifier);
  if(srp->password)
    BigIntegerClearFree(srp->password);
  if(srp->pubkey)
    BigIntegerFree(srp->pubkey);
  if(srp->secret)
    BigIntegerClearFree(srp->secret);
  if(srp->u)
    BigIntegerFree(srp->u);
  if(srp->key)
    BigIntegerClearFree(srp->key);
  if(srp->bctx)
    BigIntegerCtxFree(srp->bctx);
  if(srp->ex_data)
    cstr_clear_free(srp->ex_data);
  free(srp);
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_set_server_lookup(SRP * srp, SRP_SERVER_LOOKUP * lookup)
{
  srp->slu = lookup;
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_set_client_param_verify_cb(SRP * srp, SRP_CLIENT_PARAM_VERIFY_CB cb)
{
  srp->param_cb = cb;
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_set_username(SRP * srp, const char * username)
{
  cstr_set(srp->username, username);
  if(srp->slu)
    return SRP_SERVER_do_lookup(srp->slu, srp, srp->username);
  else
    return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_set_user_raw(SRP * srp, const unsigned char * user, int userlen)
{
  cstr_setn(srp->username, (const char *)user, userlen);
  if(srp->slu)
    return SRP_SERVER_do_lookup(srp->slu, srp, srp->username);
  else
    return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_set_params(SRP * srp, const unsigned char * modulus, int modlen,
	       const unsigned char * generator, int genlen,
	       const unsigned char * salt, int saltlen)
{
  SRP_RESULT rc;

  if(modulus == NULL || generator == NULL || salt == NULL)
    return SRP_ERROR;

  /* Set fields in SRP context */
  srp->modulus = BigIntegerFromBytes(modulus, modlen);
  if(srp->flags & SRP_FLAG_MOD_ACCEL)
    srp->accel = BigIntegerModAccelNew(srp->modulus, srp->bctx);
  srp->generator = BigIntegerFromBytes(generator, genlen);
  if(srp->salt == NULL)
    srp->salt = cstr_new();
  cstr_setn(srp->salt, (const char *)salt, saltlen);

  /* Now attempt to validate parameters */
  if(BigIntegerBitLen(srp->modulus) < SRP_get_modulus_min_bits())
    return SRP_ERROR;

  if(srp->param_cb) {
    rc = (*srp->param_cb)(srp, modulus, modlen, generator, genlen);
    if(!SRP_OK(rc))
      return rc;
  }

  return (*srp->meth->params)(srp, modulus, modlen, generator, genlen,
			      salt, saltlen);
}

_TYPE( SRP_RESULT )
SRP_set_authenticator(SRP * srp, const unsigned char * a, int alen)
{
  return (*srp->meth->auth)(srp, a, alen);
}

_TYPE( SRP_RESULT )
SRP_set_auth_password(SRP * srp, const char * password)
{
  return (*srp->meth->passwd)(srp, (const unsigned char *)password,
			      strlen(password));
}

_TYPE( SRP_RESULT )
SRP_set_auth_password_raw(SRP * srp,
			  const unsigned char * password, int passlen)
{
  return (*srp->meth->passwd)(srp, password, passlen);
}

_TYPE( SRP_RESULT )
SRP_gen_pub(SRP * srp, cstr ** result)
{
  return (*srp->meth->genpub)(srp, result);
}

_TYPE( SRP_RESULT )
SRP_add_ex_data(SRP * srp, const unsigned char * data, int datalen)
{
  cstr_appendn(srp->ex_data, (const char *)data, datalen);
  return SRP_SUCCESS;
}

_TYPE( SRP_RESULT )
SRP_compute_key(SRP * srp, cstr ** result,
		const unsigned char * pubkey, int pubkeylen)
{
  return (*srp->meth->key)(srp, result, pubkey, pubkeylen);
}

_TYPE( SRP_RESULT )
SRP_verify(SRP * srp, const unsigned char * proof, int prooflen)
{
  return (*srp->meth->verify)(srp, proof, prooflen);
}

_TYPE( SRP_RESULT )
SRP_respond(SRP * srp, cstr ** proof)
{
  return (*srp->meth->respond)(srp, proof);
}

#if 0

#include "t_pwd.h"

/* Convenience functions - currently depend on classic EPS API */

_TYPE( SRP_RESULT )
SRP_server_init_user(SRP * srp, const char * username)
{
  SRP_set_server_lookup(srp, SRP_SERVER_system_lookup());
  return SRP_set_username(srp, username);
}

static SRP_RESULT
slu_nop(SRP_SERVER_LOOKUP * slu)
{
  return SRP_SUCCESS;
}

static SRP_RESULT
slu_sys_lookup(SRP_SERVER_LOOKUP * slu, SRP * srp, cstr * username)
{
  struct t_passwd * p;

  p = gettpnam(username->data);
  if(p == NULL)
    return SRP_ERROR;
  if(!SRP_OK(SRP_set_params(srp, p->tc.modulus.data, p->tc.modulus.len,
			    p->tc.generator.data, p->tc.generator.len,
			    p->tp.salt.data, p->tp.salt.len)))
    return SRP_ERROR;
  if(!SRP_OK(SRP_set_authenticator(srp,
				   p->tp.password.data, p->tp.password.len)))
    return SRP_ERROR;

  return SRP_SUCCESS;
}

static struct srp_s_lu_meth_st srp_s_lu_sys_meth = {
  "SRP/EPS system lookup",
  slu_nop,
  slu_nop,
  slu_sys_lookup,
  NULL
};

static struct srp_server_lu_st srp_s_sys_lookup = {
  &srp_s_lu_sys_meth,
  NULL
};

_TYPE( SRP_SERVER_LOOKUP * )
SRP_SERVER_system_lookup()
{
  return &srp_s_sys_lookup;
}

extern int t_isprime(BigInteger);

_TYPE( SRP_RESULT )
SRP_CLIENT_builtin_param_verify_cb(SRP * srp, const unsigned char * mod, int modlen, const unsigned char * gen, int genlen)
{
  struct t_preconf * tpc;
  int i;

  for(i = 0; i < t_getprecount(); ++i) {
    tpc = t_getpreparam(i);
    if(tpc->modulus.len == modlen && tpc->generator.len == genlen &&
       memcmp(tpc->modulus.data, mod, modlen) == 0 &&
       memcmp(tpc->generator.data, gen, genlen) == 0) {
      return SRP_SUCCESS;		/* Match found, done */
    }
  }
  return SRP_ERROR;
}

_TYPE( SRP_RESULT )
SRP_CLIENT_compat_param_verify_cb(SRP * srp, const unsigned char * mod, int modlen, const unsigned char * gen, int genlen)
{
  SRP_RESULT rc;
  BigInteger q, r;

  rc = SRP_CLIENT_builtin_param_verify_cb(srp, mod, modlen, gen, genlen);
  if(SRP_OK(rc))
    return rc;

  /* Now do a safe-primality and generator test */
  if(!t_isprime(srp->modulus))
    return SRP_ERROR;

  rc = SRP_ERROR;
  /* q = (n-1)/2 */
  q = BigIntegerFromInt(0);
  BigIntegerSubInt(q, srp->modulus, 1);
  BigIntegerDivInt(q, q, 2, srp->bctx);
  if(t_isprime(q) && BigIntegerCmp(srp->modulus, srp->generator) > 0) {
    r = BigIntegerFromInt(0);		/* Check g^q == -1 (mod n) */
    BigIntegerModExp(r, srp->generator, q, srp->modulus, srp->bctx, srp->accel);
    BigIntegerAddInt(r, r, 1);
    if(BigIntegerCmp(r, srp->modulus) == 0) {
      BigIntegerAddInt(r, srp->generator, 1);
      if(BigIntegerCmp(r, srp->modulus) != 0)
	rc = SRP_SUCCESS;
    }
    BigIntegerFree(r);
  }
  BigIntegerFree(q);
  return rc;
}

_TYPE( SRP_RESULT )
SRP_CLIENT_default_param_verify_cb(SRP * srp, const unsigned char * mod, int modlen, const unsigned char * gen, int genlen)
{
  return SRP_CLIENT_builtin_param_verify_cb(srp, mod, modlen, gen, genlen);
}

_TYPE( SRP_RESULT )
SRP_use_engine(const char * engine)
{
  if(BigIntegerOK(BigIntegerUseEngine(engine)))
    return SRP_SUCCESS;
  else
    return SRP_ERROR;
}
#endif
