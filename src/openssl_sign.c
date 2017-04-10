/* sources:
	
	http://stackoverflow.com/a/12661380
	https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying
	
*/


/* gcc -g3 -O1 -Wall -std=c99 -I/usr/local/ssl/darwin/include t-rsa.c /usr/local/ssl/darwin/lib/libcrypto.a -o t-rsa.exe */
/* gcc -g2 -Os -Wall -DNDEBUG=1 -std=c99 -I/usr/local/ssl/darwin/include t-rsa.c /usr/local/ssl/darwin/lib/libcrypto.a -o t-rsa.exe */
 
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


//typedef unsigned char byte;
#define UNUSED(x) ((void)x)
const char hn[] = "SHA256";
const char* pcszPassphrase = "open sezamee";

int verifyLANbeacon(const unsigned char* msg, size_t mlen)
{
	int rc;
	
	OpenSSL_add_all_algorithms();
	
	/* Sign and Verify HMAC keys */
	EVP_PKEY *skey = NULL, *vkey = NULL;
	
	rc = read_keys(&skey, &vkey);
	
	assert(rc == 0);
	if(rc != 0)
		exit(1);
	
	assert(skey != NULL);
	if(skey == NULL)
		exit(1);
	
	assert(vkey != NULL);
	if(vkey == NULL)
		exit(1);
	
/*	unsigned char* sig_buffer = malloc(260);
	FILE*	 signatureFile = fopen("BeaconSignature","r");
	size_t result = fread (sig_buffer,1,260,signatureFile);
	
	print_it("Signature", sig_buffer, result);
	
	printf("sigSize read: %zu\nmlen rec: %zu\n", result, mlen);
	
	fclose(signatureFile);
*/	

	print_it("empfangene Version", &msg[mlen - 256], 256);

	/* Using the vkey or verifying key */
	rc = verify_it(msg, mlen - 256, &msg[mlen - 256], 256, vkey);
	if(rc == 0) {
		printf("Verified signature on receiver side\n");
	} else {
		printf("Failed to verify signature on receiver side, return code %d\n", rc);
	}
	
	if(skey)
		EVP_PKEY_free(skey);
	
	if(vkey)
		EVP_PKEY_free(vkey);
	
//	if(sig_buffer)
//		OPENSSL_free(sig_buffer);
	
	return 0;
}

/////////////////////

int signLANbeacon(unsigned char** sig, size_t* slen, const unsigned char* msg, size_t mlen)
{
	int rc;
	
	printf("Testing RSA functions with EVP_DigestSign and EVP_DigestVerify\n");
	
	OpenSSL_add_all_algorithms();
	
	/* Sign and Verify HMAC keys */
	EVP_PKEY *skey = NULL, *vkey = NULL;
	
//	rc = make_keys(&skey, &vkey);
	rc = read_keys(&skey, &vkey);
	
	assert(rc == 0);
	if(rc != 0)
		exit(1);
	
	assert(skey != NULL);
	if(skey == NULL)
		exit(1);
	
	assert(vkey != NULL);
	if(vkey == NULL)
		exit(1);
	
	/* Using the skey or signing key */
	rc = sign_it(msg, mlen, sig, slen, skey);
	assert(rc == 0);
	if(rc == 0) {
		printf("Created signature\n");
	} else {
		printf("Failed to create signature, return code %d\n", rc);
		exit(1); /* Should cleanup here */
	}
	
	print_it("Signature", *sig, *slen);
	
	printf("sigsize write%zu\n mlen send %zu\n", *slen, mlen);
	
	FILE*	 signatureFile = fopen("BeaconSignature","w");
	fwrite(*sig, *slen, 1, signatureFile);
	
	fclose(signatureFile);
	
	/* Using the vkey or verifying key */
	rc = verify_it(msg, mlen, *sig, *slen, vkey);
	if(rc == 0) {
		printf("Verified signature on sender side\n");
	} else {
		printf("Failed to verify signature on sender side, return code %d\n", rc);
	}
	
//	if(skey)
//		EVP_PKEY_free(skey);
	
//	if(vkey)
//		EVP_PKEY_free(vkey);
	
//	if(sig)
//		OPENSSL_free(sig);

	print_it("dubdub Signature", *sig, *slen);
	
	print_it("qqqdubdub Signature", *sig, *slen);
	
	printf ("%p\n", *sig);


	return 0;
}

int sign_it(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen, EVP_PKEY* pkey)
{
	/* Returned to caller */
	int result = -1;
	
	if(!msg || !mlen || !sig || !pkey) {
		assert(0);
		return -1;
	}
	
	if(*sig)
		OPENSSL_free(*sig);
	
	*sig = NULL;
	*slen = 0;
	
	EVP_MD_CTX* ctx = NULL;
	
	do
	{
		ctx = EVP_MD_CTX_create();
//		assert(ctx != NULL);
		if(ctx == NULL) {
			printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		const EVP_MD* md = EVP_get_digestbyname(hn);
//		assert(md != NULL);
		if(md == NULL) {
			printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		int rc = EVP_DigestInit_ex(ctx, md, NULL);
//		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
//		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		rc = EVP_DigestSignUpdate(ctx, msg, mlen);
//		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		size_t req = 0;
		rc = EVP_DigestSignFinal(ctx, NULL, &req);
//		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
//		assert(req > 0);
		if(!(req > 0)) {
			printf("EVP_DigestSignFinal failed (2), error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		*sig = OPENSSL_malloc(req);
//		assert(*sig != NULL);
		if(*sig == NULL) {
			printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		*slen = req;
		rc = EVP_DigestSignFinal(ctx, *sig, slen);
//		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
			break; /* failed */
		}
		
//		assert(req == *slen);
		if(rc != 1) {
			printf("EVP_DigestSignFinal failed, mismatched signature sizes %ld, %ld", req, *slen);
			break; /* failed */
		}
		
		result = 0;
		
	} while(0);
	
	if(ctx) {
		EVP_MD_CTX_destroy(ctx);
		ctx = NULL;
	}
	
	return !!result;
}

int verify_it(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen, EVP_PKEY* pkey)
{
	/* Returned to caller */
	int result = -1;
	if(!msg || !mlen || !sig || !slen || !pkey) {
		assert(0);
		return -1;
	}
	
	EVP_MD_CTX* ctx = NULL;
	
	do
	{
		ctx = EVP_MD_CTX_create();
		assert(ctx != NULL);
		if(ctx == NULL) {
			printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		const EVP_MD* md = EVP_get_digestbyname(hn);
		assert(md != NULL);
		if(md == NULL) {
			printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		int rc = EVP_DigestInit_ex(ctx, md, NULL);
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		rc = EVP_DigestVerifyInit(ctx, NULL, md, NULL, pkey);
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestVerifyInit failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		rc = EVP_DigestVerifyUpdate(ctx, msg, mlen);
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestVerifyUpdate failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		/* Clear any errors for the call below */
		ERR_clear_error();
		
		rc = EVP_DigestVerifyFinal(ctx, (unsigned char *) sig, slen);
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_DigestVerifyFinal failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		result = 0;
		
	} while(0);
	
	if(ctx) {
		EVP_MD_CTX_destroy(ctx);
		ctx = NULL;
	}
	
	return !!result;

}

void print_it(const char* label, const unsigned char* buff, size_t len)
{
	if(!buff || !len)
		return;
	
	if(label)
		printf("%s: ", label);
	
	for(size_t i=0; i < len; ++i)
		printf("%02X", buff[i]);
	
	printf("\n");
}

int read_keys(EVP_PKEY** skey, EVP_PKEY** vkey) {
	
	FILE*	 pFile	= NULL;
	int iRet;

	if((pFile = fopen("privkey.pem","rt")) && 
	   (*skey = PEM_read_PrivateKey(pFile,NULL,passwd_callback,(void*)pcszPassphrase)))
	{
		fprintf(stderr,"Private key read.\n");
	}
	else
	{
		fprintf(stderr,"Cannot read \"privkey.pem\".\n");
		ERR_print_errors_fp(stderr);
		iRet = EXIT_FAILURE;
	}
	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	if((pFile = fopen("pubkey.pem","rt")) && 
	   (*vkey = PEM_read_PUBKEY(pFile,NULL,NULL,NULL)))
	{
		fprintf(stderr,"Public key read.\n");
	}
	else
	{
		fprintf(stderr,"Cannot read \"pubkey.pem\".\n");
		ERR_print_errors_fp(stderr);
		iRet = EXIT_FAILURE;
	}
	return 0; 
	
}

/*
int read_pubkey(EVP_PKEY** vkey) {
	
	FILE*	 pFile	= NULL;
	int iRet;
	
	if((pFile = fopen("pubkey.pem","rt")) && 
	   (*vkey = PEM_read_PUBKEY(pFile,NULL,NULL,NULL)))
	{
		fprintf(stderr,"Public key read.\n");
	}
	else
	{
		fprintf(stderr,"Cannot read \"pubkey.pem\".\n");
		ERR_print_errors_fp(stderr);
		iRet = EXIT_FAILURE;
	}
	return 0; 
	
}
*/
int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey)
{
	int result = -1;
	
	if(!skey || !vkey)
		return -1;
	
	if(*skey != NULL) {
		EVP_PKEY_free(*skey);
		*skey = NULL;
	}
	
	if(*vkey != NULL) {
		EVP_PKEY_free(*vkey);
		*vkey = NULL;
	}
	
	RSA* rsa = NULL;
	
	do
	{
		*skey = EVP_PKEY_new();
		assert(*skey != NULL);
		if(*skey == NULL) {
			printf("EVP_PKEY_new failed (1), error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		*vkey = EVP_PKEY_new();
		assert(*vkey != NULL);
		if(*vkey == NULL) {
			printf("EVP_PKEY_new failed (2), error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		rsa = RSA_generate_key(2048, RSA_F4, NULL, NULL);
		assert(rsa != NULL);
		if(rsa == NULL) {
			printf("RSA_generate_key failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		/* Set signing key */
		int rc = EVP_PKEY_assign_RSA(*skey, RSAPrivateKey_dup(rsa));
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_PKEY_assign_RSA (1) failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		/* Sanity check. Verify private exponent is present */
		/* assert(EVP_PKEY_get0_RSA(*skey)->d != NULL); */

		/* Set verifier key */
		rc = EVP_PKEY_assign_RSA(*vkey, RSAPublicKey_dup(rsa));
		assert(rc == 1);
		if(rc != 1) {
			printf("EVP_PKEY_assign_RSA (2) failed, error 0x%lx\n", ERR_get_error());
			break; /* failed */
		}
		
		/* Sanity check. Verify private exponent is missing */
		/* assert(EVP_PKEY_get0_RSA(*vkey)->d == NULL); */
		
		result = 0;
		
	} while(0);
	
	////////////////////////////////////
	
	const EVP_CIPHER* pCipher = NULL;

	FILE*	 pFile	= NULL;
	
	int iRet = EXIT_SUCCESS;
		
	if((pFile = fopen("privkey.pem","wt")) && (pCipher = EVP_aes_256_cbc()))
	{

		if(!PEM_write_PrivateKey(pFile,*skey,pCipher,
								(unsigned char*)pcszPassphrase,
								(int)strlen(pcszPassphrase),NULL,NULL))
		{
			fprintf(stderr,"PEM_write_PrivateKey failed.\n");
			ERR_print_errors_fp(stderr);
			iRet = EXIT_FAILURE;
		}
		fclose(pFile);
		pFile = NULL;
		if(iRet == EXIT_SUCCESS)
		{
			if((pFile = fopen("pubkey.pem","wt")) && PEM_write_PUBKEY(pFile,*vkey))
				fprintf(stderr,"Both keys saved.\n");
			else
			{
				ERR_print_errors_fp(stderr);
				iRet = EXIT_FAILURE;
			}
			if(pFile)
			{
				fclose(pFile);
				pFile = NULL;
			}
		}
	}
	
	EVP_PKEY_free(*skey);
	skey = NULL;
	EVP_PKEY_free(*vkey);
	vkey = NULL;
			

	
	///////////////////////////////
	
	if(rsa) {
		RSA_free(rsa);
		rsa = NULL;
	}
	
	return !!result;
}


int passwd_callback(char *pcszBuff,int size,int rwflag, void *pPass)
{
	size_t unPass = strlen((char*)pPass);
	if(unPass > (size_t)size)
		unPass = (size_t)size;
	memcpy(pcszBuff, pPass, unPass);
	return (int)unPass;
}

