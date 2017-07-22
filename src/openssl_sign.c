// sources:
//	http://stackoverflow.com/a/12661380
//	https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying

#include <libintl.h>
#include <locale.h>
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
const char hn[] = "SHA256";

#define KEY_READ_PROBLEM 0b1
#define VERFIY_PROBLEM 0b01
#define NO_PRIVATE_KEY 0b001
#define NO_PUBLIC_KEY 0b0001
#define PROBLEM_IN_SIGN_CALL 0b00001
#define PROBLEM_IN_VERIFY_CALL 0b000001

#define SIG_LEN 256


int verifylanbeacon(const unsigned char* msg, size_t mlen, 
					struct open_ssl_keys *lanbeacon_keys)
{
	int rc;
	OpenSSL_add_all_algorithms();
	int result = EXIT_SUCCESS;

	// Sign and Verify HMAC keys
	EVP_PKEY *skey = NULL, *vkey = NULL;
	rc = read_keys(&skey, &vkey, lanbeacon_keys);

	if(rc & NO_PUBLIC_KEY == NO_PUBLIC_KEY) {
		printf(_("Could not read public key at specified path %s.\n"), 
			lanbeacon_keys->path_To_Verifying_Key);
		return(rc);
	}

	// Using the vkey or verifying key
//	rc = verify_it(msg, mlen - 256, &msg[mlen - 256], 256, vkey);

	size_t slen = 256;
	size_t messageWithoutSignatureLength = mlen - 256;
	const unsigned char* sig = &msg[messageWithoutSignatureLength];
FILE *binBeacon = fopen("msgForVerify","w");
fwrite(msg, messageWithoutSignatureLength, 1, binBeacon);
fclose(binBeacon);

	if(!msg || !messageWithoutSignatureLength || !sig || !slen || !vkey) {
		puts(_("Problem in function \"verify_it\". "));
		return PROBLEM_IN_VERIFY_CALL;
	}

	EVP_MD_CTX* ctx = NULL;

	ctx = EVP_MD_CTX_create();
	if(ctx == NULL) {
		printf(_("EVP_MD_CTX_create failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	const EVP_MD* md = EVP_get_digestbyname(hn);
	if(md == NULL) {
		printf(_("EVP_get_digestbyname failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestInit_ex(ctx, md, NULL);
	if(rc != 1) {
		printf(_("EVP_DigestInit_ex failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestVerifyInit(ctx, NULL, md, NULL, vkey);
	if(rc != 1) {
		printf(_("EVP_DigestVerifyInit failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestVerifyUpdate(ctx, msg, messageWithoutSignatureLength);
	if(rc != 1) {
		printf(_("EVP_DigestVerifyUpdate failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	// Clear any errors for the call below
	ERR_clear_error();

printf("mlen %i   \n", mlen );
	rc = EVP_DigestVerifyFinal(ctx, (unsigned char *) sig, slen);
	if(rc != 1) {
		printf(_("EVP_DigestVerifyFinal failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}
	else printf(_("Verified signature on receiver side\n"));

	result = 0;

	if(ctx) {
		EVP_MD_CTX_destroy(ctx);
		ctx = NULL;
	}

	if(skey)
		EVP_PKEY_free(skey);

	if(vkey)
		EVP_PKEY_free(vkey);

	return rc & result;
}


int signlanbeacon(unsigned char** sig, size_t* slen, const unsigned char* msg, 
				size_t mlen, struct open_ssl_keys *lanbeacon_keys)
{
FILE *binBeacon = fopen("msgForSign","w");
fwrite(msg, mlen, 1, binBeacon);
fclose(binBeacon);
	int rc;
	int result = EXIT_SUCCESS;

	OpenSSL_add_all_algorithms();

	// Sign and verify keys
	EVP_PKEY *skey = NULL, *vkey = NULL;

	rc = read_keys(&skey, &vkey, lanbeacon_keys);

	if(rc & NO_PRIVATE_KEY == NO_PRIVATE_KEY) {
		printf(_("Could not read private key at specified path %s. Maybe the password or path are wrong?\n"), 
			lanbeacon_keys->path_To_Verifying_Key);
		
		if (lanbeacon_keys->generate_keys) {
			puts("Key pair will be created. ");
			rc = make_keys(&skey, &vkey, lanbeacon_keys);
		}
	}

	if(skey == NULL)
		exit(NO_PRIVATE_KEY);

	// Using the skey or signing key
//	rc = sign_it(msg, mlen, sig, slen, skey);

	if(!msg || !mlen || !sig || !skey) {
		puts(_("Problem in function \"sign_it\". "));
		return PROBLEM_IN_SIGN_CALL;
	}

	if(*sig)
		OPENSSL_free(*sig);
	*sig = NULL;
	*slen = 0;

	EVP_MD_CTX* ctx = NULL;

	ctx = EVP_MD_CTX_create();
	if(ctx == NULL) {
		printf(_("EVP_MD_CTX_create failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	const EVP_MD* md = EVP_get_digestbyname(hn);
	if(md == NULL) {
		printf(_("EVP_get_digestbyname failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestInit_ex(ctx, md, NULL);
	if(rc != 1) {
		printf(_("EVP_DigestInit_ex failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestSignInit(ctx, NULL, md, NULL, skey);
	if(rc != 1) {
		printf(_("EVP_DigestSignInit failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rc = EVP_DigestSignUpdate(ctx, msg, mlen);
	if(rc != 1) {
		printf(_("EVP_DigestSignUpdate failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	size_t req = 0;
	rc = EVP_DigestSignFinal(ctx, NULL, &req);
	if(rc != 1) {
		printf(_("EVP_DigestSignFinal failed (1), error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	if(!(req > 0)) {
		printf(_("EVP_DigestSignFinal failed (2), error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	*sig = OPENSSL_malloc(req);
	if(*sig == NULL) {
		printf(_("OPENSSL_malloc failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	*slen = req;
	rc = EVP_DigestSignFinal(ctx, *sig, slen);
	if(rc != 1) {
		printf(_("EVP_DigestSignFinal failed, return code %d, error 0x%lx\n, "
			"mismatched signature sizes %ld, %ld"), rc, ERR_get_error(), req, *slen);
		return(ERR_get_error());
	}
	else printf(_("Created signature\n"));

	if(ctx) {
		EVP_MD_CTX_destroy(ctx);
		ctx = NULL;
	}

	return result;
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

int read_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys) {

	FILE*	 pFile	= NULL;
	int iRet = EXIT_SUCCESS;

	// only open signing key, if in sender mode
	if (lanbeacon_keys->sender_or_receiver_mode == SENDER_MODE) {
		if ((pFile = fopen(lanbeacon_keys->path_To_Signing_Key,"rt")) 
		&&(*skey = PEM_read_PrivateKey(pFile, NULL, passwd_callback, (void*)lanbeacon_keys->pcszPassphrase)))
		{
			fprintf(stderr,_("Private key read.\n"));
		}
		else
		{
			fprintf(stderr,_("Cannot read \"%s\".\n"), lanbeacon_keys->path_To_Signing_Key);
			ERR_print_errors_fp(stderr);
			iRet = iRet | NO_PRIVATE_KEY;
		}
		if(pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}
	}
	
	// only open verifying key, if in listener mode
	if (lanbeacon_keys->sender_or_receiver_mode == RECEIVER_MODE) {
		if((pFile = fopen(lanbeacon_keys->path_To_Verifying_Key,"rt")) &&
		   (*vkey = PEM_read_PUBKEY(pFile,NULL,NULL,NULL)))
		{
			fprintf(stderr,_("Public key read.\n"));
		}
		else
		{
			fprintf(stderr,_("Cannot read \"%s\".\n"), lanbeacon_keys->path_To_Verifying_Key);
			ERR_print_errors_fp(stderr);
			iRet = iRet | NO_PUBLIC_KEY;
		}
	}

	return iRet;

}


int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys)
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

	*skey = EVP_PKEY_new();
	if(*skey == NULL) {
		printf(_("EVP_PKEY_new failed (1), error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	*vkey = EVP_PKEY_new();
	if(*vkey == NULL) {
		printf(_("EVP_PKEY_new failed (2), error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	rsa = RSA_generate_key(2048, RSA_F4, NULL, NULL);
	if(rsa == NULL) {
		printf(_("RSA_generate_key failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	// Set signing key
	int rc = EVP_PKEY_assign_RSA(*skey, RSAPrivateKey_dup(rsa));
	if(rc != 1) {
		printf(_("EVP_PKEY_assign_RSA (1) failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	// Sanity check. Verify private exponent is present
	// assert(EVP_PKEY_get0_RSA(*skey)->d != NULL);

	// Set verifier key
	rc = EVP_PKEY_assign_RSA(*vkey, RSAPublicKey_dup(rsa));
	if(rc != 1) {
		printf(_("EVP_PKEY_assign_RSA (2) failed, error 0x%lx\n"), ERR_get_error());
		return(ERR_get_error());
	}

	// Sanity check. Verify private exponent is missing
	// assert(EVP_PKEY_get0_RSA(*vkey)->d == NULL);

	result = 0;


	const EVP_CIPHER* pCipher = NULL;

	FILE* pFile = NULL;

	int iRet = EXIT_SUCCESS;

	if((pFile = fopen(lanbeacon_keys->path_To_Signing_Key,"wt")) 
	&& (pCipher = EVP_aes_256_cbc()))
	{

		if(!PEM_write_PrivateKey(pFile,*skey,pCipher,
								(unsigned char*)lanbeacon_keys->pcszPassphrase,
								(int)strlen(lanbeacon_keys->pcszPassphrase),NULL,NULL))
		{
			fprintf(stderr,_("PEM_write_PrivateKey failed.\n"));
			ERR_print_errors_fp(stderr);
			iRet = EXIT_FAILURE;
		}
		fclose(pFile);
		pFile = NULL;
		if(iRet == EXIT_SUCCESS)
		{
			if((pFile = fopen(lanbeacon_keys->path_To_Verifying_Key,"wt")) 
			&& PEM_write_PUBKEY(pFile,*vkey))
				fprintf(stderr,_("Both keys saved.\n"));
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

