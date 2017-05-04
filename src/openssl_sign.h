#ifndef OPENSSL_SIGN_H
#define OPENSSL_SIGN_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include "openssl_sign.h"
#include "define.h"

struct open_ssl_keys {
	char path_To_Verifying_Key[KEY_PATHLENGTH_MAX];
	char path_To_Signing_Key[KEY_PATHLENGTH_MAX];
};

/* Returns 0 for success, non-0 otherwise */
int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys);

/* Returns 0 for success, non-0 otherwise */
int sign_it(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen, EVP_PKEY* pkey);

/* Returns 0 for success, non-0 otherwise */
int verify_it(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen, EVP_PKEY* pkey);

/* Prints a buffer to stdout. Label is optional */
void print_it(const char* label, const unsigned char* buff, size_t len);

int passwd_callback(char *pcszBuff,int size,int rwflag, void *pPass);

int signlanbeacon(unsigned char** sig, size_t* slen, const unsigned char* msg, size_t qqlen, struct open_ssl_keys *lanbeacon_keys); 

int read_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys);

// int read_pubkey(EVP_PKEY** vkey);

int verifylanbeacon(const unsigned char* msg, size_t mlen, struct open_ssl_keys *lanbeacon_keys);

#endif
