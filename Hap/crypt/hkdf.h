/*
 * hkdf.h
 *
 *  Created on: Mar 2, 2016
 *      Author: Gera Kazakov
 *
 *	HAP daemon
 *		HKDF function
 */

#ifndef HKDF_H_
#define HKDF_H_

#include <hmac_sha2.h>

void hkdf(
    const unsigned char *salt, size_t salt_len,
    const unsigned char *key, size_t key_len,
    const unsigned char *info, size_t info_len,
    unsigned char *okm, unsigned int okm_len);

#endif /* HKDF_H_ */
