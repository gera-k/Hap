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

void sha2_test(void);
int srp_test(void);

#endif
