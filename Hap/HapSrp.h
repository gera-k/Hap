#ifndef _HAP_SRP_H_
#define _HAP_SRP_H_

// Interface to Secure Remote Password protocol

namespace Hap
{
}

extern const unsigned char srp_modulus[];
extern int sizeof_srp_modulus;
extern const unsigned char srp_generator[];
extern int sizeof_srp_generator;
extern const unsigned char srp_salt[];
extern int sizeof_srp_salt;

void sha2_test(void);
void srp_test(void);
int srp_test2(void);
int mp_test();

#endif
