#include "HapSrp.h"

const unsigned char srp_modulus[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
	0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
	0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
	0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22,
	0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
	0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B,
	0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
	0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
	0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
	0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B,
	0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
	0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5,
	0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
	0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
	0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
	0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A,
	0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
	0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96,
	0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
	0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
	0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
	0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C,
	0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
	0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03,
	0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
	0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
	0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
	0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5,
	0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
	0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D,
	0xAD, 0x33, 0x17, 0x0D, 0x04, 0x50, 0x7A, 0x33,
	0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64,
	0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A,
	0x8A, 0xEA, 0x71, 0x57, 0x5D, 0x06, 0x0C, 0x7D,
	0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7,
	0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7,
	0x1E, 0x8C, 0x94, 0xE0, 0x4A, 0x25, 0x61, 0x9D,
	0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B,
	0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64,
	0xD8, 0x76, 0x02, 0x73, 0x3E, 0xC8, 0x6A, 0x64,
	0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C,
	0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C,
	0x77, 0x09, 0x88, 0xC0, 0xBA, 0xD9, 0x46, 0xE2,
	0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31,
	0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E,
	0x4B, 0x82, 0xD1, 0x20, 0xA9, 0x3A, 0xD2, 0xCA,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
int sizeof_srp_modulus = sizeof(srp_modulus);

const unsigned char srp_generator[] = { 0x05 };
int sizeof_srp_generator = sizeof(srp_generator);

const unsigned char srp_salt[] = { 
	0xBE, 0xB2, 0x53, 0x79, 0xD1, 0xA8, 0x58, 0x1E,
	0xB5, 0xA7, 0x27, 0x67, 0x3A, 0x24, 0x41, 0xEE
};
int sizeof_srp_salt = sizeof(srp_salt);

#include "sha2.c"

#include "cstr.c"
#include "t_math.c" 
#include "srp.c"
#include "srp6_server.c"
#include "srp6_client.c"
//#include "srptest.c"

#if 0
#include "tommath_class.h"
#include "bncore.c"
#include "bn_mp_init.c"
#include "bn_mp_init_size.c"
#include "bn_mp_init_copy.c"
#include "bn_mp_clear.c"
#include "bn_mp_set_int.c"
#include "bn_mp_zero.c"
#include "bn_mp_clamp.c"
#include "bn_mp_mul_2d.c"
#include "bn_mp_grow.c"
#include "bn_mp_copy.c"
#include "bn_mp_lshd.c"
#include "bn_mp_rshd.c"
#include "bn_mp_cmp.c"
#include "bn_mp_cmp_mag.c"
#include "bn_mp_cmp_d.c"
#include "bn_mp_add.c"
#include "bn_mp_sub.c"
#include "bn_mp_mul.c"
#include "bn_mp_mod.c"
#include "bn_mp_mod_d.c"
#include "bn_mp_mod_2d.c"
#include "bn_mp_exch.c"
#include "bn_mp_div.c"
#include "bn_mp_div_2d.c"
#include "bn_mp_add_d.c"
#include "bn_mp_sub_d.c"
#include "bn_mp_mul_d.c"
#include "bn_mp_mulmod.c"
#include "bn_mp_exptmod.c"
#include "bn_mp_div_d.c"
#include "bn_prime_tab.c"
#include "bn_mp_prime_is_prime.c"
#include "bn_mp_prime_is_divisible.c"
#include "bn_mp_prime_miller_rabin.c"
#include "bn_mp_count_bits.c"
#include "bn_mp_unsigned_bin_size.c"
#include "bn_mp_read_signed_bin.c"
#include "bn_mp_read_unsigned_bin.c"
#include "bn_mp_to_unsigned_bin.c"
#include "bn_mp_toradix.c"
#include "bn_s_mp_add.c"
#include "bn_s_mp_sub.c"
#include "bn_mp_set.c"
#include "bn_mp_cnt_lsb.c"
#include "bn_mp_sqr.c"
#include "bn_mp_sqrmod.c"
#include "bn_mp_radix_smap.c"
#include "bn_reverse.c"
#include "bn_mp_clear_multi.c"
#include "bn_mp_abs.c"
#include "bn_mp_div_3.c"
#include "bn_mp_invmod.c"
#include "bn_mp_dr_is_modulus.c"
#include "bn_mp_reduce_is_2k.c"
#include "bn_mp_reduce_is_2k_l.c"
#include "bn_fast_s_mp_mul_digs.c"
#include "bn_s_mp_mul_digs.c"
#include "bn_fast_s_mp_sqr.c"
#include "bn_s_mp_sqr.c"
#include "bn_mp_karatsuba_mul.c"
#include "bn_mp_toom_mul.c"
#include "bn_mp_karatsuba_sqr.c"
#include "bn_mp_toom_sqr.c"
#include "bn_mp_exptmod_fast.c"
#include "bn_s_mp_exptmod.c"
#include "bn_mp_init_multi.c"
#include "bn_mp_div_2.c"
#include "bn_mp_mul_2.c"
#include "bn_mp_reduce_setup.c"
#include "bn_mp_reduce.c"
#include "bn_mp_montgomery_setup.c"
#include "bn_mp_montgomery_calc_normalization.c"
#include "bn_mp_montgomery_reduce.c"
#include "bn_mp_dr_setup.c"
#include "bn_mp_dr_reduce.c"
#include "bn_mp_reduce_2k_setup.c"
#include "bn_mp_reduce_2k.c"
#include "bn_mp_reduce_2k_setup_l.c"
#include "bn_mp_reduce_2k_l.c"
#include "bn_fast_mp_invmod.c"
#include "bn_mp_invmod_slow.c"
#include "bn_fast_mp_montgomery_reduce.c"
#include "bn_mp_2expt.c"
#include "bn_s_mp_mul_high_digs.c"
#include "bn_fast_s_mp_mul_high_digs.c"
#endif

#include "mpi.c"

#if 0
void
t_mgf1(unsigned char * mask, unsigned masklen, const unsigned char * seed, unsigned seedlen)
{
	SHACTX ctxt;
	unsigned i = 0;
	unsigned pos = 0;
	unsigned char cnt[4];
	unsigned char hout[SHA_DIGESTSIZE];

	while (pos < masklen) {
		cnt[0] = (i >> 24) & 0xFF;
		cnt[1] = (i >> 16) & 0xFF;
		cnt[2] = (i >> 8) & 0xFF;
		cnt[3] = i & 0xFF;
		SHAInit(&ctxt);
		SHAUpdate(&ctxt, seed, seedlen);
		SHAUpdate(&ctxt, cnt, 4);

		if (pos + SHA_DIGESTSIZE > masklen) {
			SHAFinal(hout, &ctxt);
			memcpy(mask + pos, hout, masklen - pos);
			pos = masklen;
		}
		else {
			SHAFinal(mask + pos, &ctxt);
			pos += SHA_DIGESTSIZE;
		}

		++i;
	}

	memset(hout, 0, sizeof(hout));
	memset((unsigned char *)&ctxt, 0, sizeof(ctxt));
}
#endif

//CLIENT_CTXP(srp)->k, RFC2945_KEY_LEN, s->data, s->length
void
t_mgf1(unsigned char * mask, unsigned masklen, const unsigned char * seed, unsigned seedlen)
{
	SHACTX ctxt;
	SHAInit(&ctxt);
	SHAUpdate(&ctxt, seed, seedlen);
	SHAFinal(mask, &ctxt);
}


void srp_test()
{
	SRP* srp = SRP_new(SRP6a_server_method());

	SRP_set_username(srp, "alice");

	SRP_set_params(srp,
		srp_modulus, sizeof_srp_modulus,
		srp_generator, sizeof_srp_generator,
		srp_salt, sizeof_srp_salt
	);

	SRP_set_auth_password(srp, "password123");

	cstr* pub = NULL;
	SRP_gen_pub(srp, &pub);

	if (pub)
		cstr_free(pub);

	SRP_free(srp);
}

/*
* Convert a string of bytes to their hex representation
*/
static char hexbuf[1024];
static int hblen = sizeof(hexbuf);
char *t_tohex(const unsigned char * src, unsigned size)
{
	int notleading = 0;

	char *chp = hexbuf;
	hexbuf[0] = '\0';
	if (size != 0) do 
	{
		if (notleading || *src != '\0') 
		{
			if (!notleading && (*src & 0xf0) == 0) 
			{
				sprintf(chp, "%.1X", *(unsigned char *)src);
				chp += 1;
			}
			else 
			{
				sprintf(chp, "%.2X", *(unsigned char *)src);
				chp += 2;
			}
			notleading = 1;
		}
		++src;
	} while (--size != 0);
	return hexbuf;
}

const char * user = "alice";
const char * pass = "password123";

int srp_test2(void)
{
	int i, vflag = 1;
	SRP * srps;
	SRP * srpc;
	cstr * t1;
	cstr * t2;
	cstr * ks;
	cstr * kc;

	SRP_initialize_library();

	/* Begin SRP-6 test (new API) */
	printf("\n*** Testing SRP-6a ***\n\n");

	t1 = NULL;
	printf("[client] sending username '%s'\n\n", user);
	printf("[client] sending password '%s'\n\n", pass);

	printf("[server] initializing session\n");
	srps = SRP_new(SRP6a_server_method());
	if (SRP_set_username(srps, user) < 0) 
	{
		printf("SRP_set_username failed\n");
		return 1;
	}
	if (SRP_set_params(srps, 
		srp_modulus, sizeof_srp_modulus,
		srp_generator, sizeof_srp_generator,
		srp_salt, sizeof_srp_salt
	) < 0) {
		printf("SRP_set_params failed\n");
		return 1;
	}
	if (SRP_set_auth_password(srps, pass) < 0) 
	{
		printf("SRP_set_authenticator failed\n");
		return 1;
	}

	printf("[client] initializing session\n");
	srpc = SRP_new(SRP6a_client_method());
	if (SRP_set_username(srpc, user) != SRP_SUCCESS) 
	{
		printf("SRP_set_username failed\n");
		return 1;
	}
	if (SRP_set_params(srpc, 
		srp_modulus, sizeof_srp_modulus,
		srp_generator, sizeof_srp_generator,
		srp_salt, sizeof_srp_salt
	) != SRP_SUCCESS) {
		printf("SRP_set_params failed\n");
		return 1;
	}
	if (SRP_gen_pub(srpc, &t1) != SRP_SUCCESS) 
	{
		printf("SRP_gen_pub failed\n");
		return 1;
	}
	if (vflag) 
	{
		BigIntegerToString(srpc->secret, hexbuf, hblen, 16);
		printf("[client private] a = %s\n", hexbuf);
	}
	printf("[client] sending A = %s\n\n", t_tohex( t1->data, t1->length));

	t2 = NULL;
	if (SRP_gen_pub(srps, &t2) != SRP_SUCCESS) 
	{
		printf("SRP_gen_pub failed\n");
		return 1;
	}
	if (vflag) 
	{
		BigIntegerToString(srps->secret, hexbuf, hblen, 16);
		printf("[server private] b = %s\n", hexbuf);
	}
	printf("[server] sending B = %s\n", t_tohex(t2->data, t2->length));
	ks = NULL;
	if (SRP_compute_key(srps, &ks, t1->data, t1->length) != SRP_SUCCESS) 
	{
		printf("SRP_compute_key (server) failed\n");
		return 1;
	}
	if (vflag) 
	{
		BigIntegerToString(srps->u, hexbuf, hblen, 16);
		printf("[server] u = %s\n", hexbuf);
		BigIntegerToString(srps->key, hexbuf, hblen, 16);
		printf("[server] raw key = %s\n", hexbuf);
	}

	printf("[server] session key = %s\n\n",
		 t_tohex(ks->data, ks->length));

	if (SRP_set_auth_password(srpc, pass) != SRP_SUCCESS) 
	{
		printf("SRP_set_authenticator failed\n");
		return 1;
	}
	kc = NULL;
	if (SRP_compute_key(srpc, &kc, t2->data, t2->length) != SRP_SUCCESS) 
	{
		printf("SRP_compute_key (client) failed\n");
		return 1;
	}
	if (vflag) 
	{
		BigIntegerToString(srpc->u, hexbuf, hblen, 16);
		printf("[client] u = %s\n", hexbuf);
		BigIntegerToString(srpc->key, hexbuf, hblen, 16);
		printf("[client] raw key = %s\n", hexbuf);
	}
	printf("[client] session key = %s\n",
		 t_tohex(kc->data, kc->length));
	if (SRP_respond(srpc, &kc) != SRP_SUCCESS) 
	{
		printf("SRP_respond failed\n");
		return 1;
	}
	printf("[client] sending client proof = %s\n\n",
		 t_tohex(kc->data, kc->length));

	i = SRP_verify(srps, kc->data, kc->length);
	printf("[server] verify status = %d (%s)\n", i,
		 (i == SRP_SUCCESS) ? "success" : "failure");
	if (i == SRP_SUCCESS) 
	{
		if (SRP_respond(srps, &ks) != SRP_SUCCESS) 
		{
			printf("SRP_respond failed\n");
			return 1;
		}
		printf("[server] sending server proof = %s\n\n",
			   t_tohex(ks->data, ks->length));

		i = SRP_verify(srpc, ks->data, ks->length);
		printf("[client] verify status = %d (%s)\n", i,
			   (i == SRP_SUCCESS) ? "success" : "failure");
	}

	SRP_free(srpc);
	SRP_free(srps);
	cstr_free(t1);
	cstr_free(t2);
	cstr_free(kc);
	cstr_free(ks);

	SRP_finalize_library();

	return 0;
}



