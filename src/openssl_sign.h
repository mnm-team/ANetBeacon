/**
 * @file openssl_sign.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief signing, verifying and key I/O  
 * 
 */

#ifndef OPENSSL_SIGN_H
#define OPENSSL_SIGN_H

#include <openssl/evp.h>
#include "openssl_sign.h"
#include "define.h"

#define SENDER_MODE 0
#define RECEIVER_MODE 1

/**
 * @brief Key locations, password and further configurations.
 */
struct open_ssl_keys {
	char path_To_Verifying_Key[KEY_PATHLENGTH_MAX+1];	/**< Specified path of public key location. */
	char path_To_Signing_Key[KEY_PATHLENGTH_MAX+1];	/**< Specified path of private key location. */
	char pcszPassphrase[1024];	/**< Specified password for private key. */
	int generate_keys;	/**< Flag that determines, if keys should be generated. */
	int sender_or_receiver_mode;	/**< Flag for corresponding client mode. */
};

/**
 * @brief Generate and save keys to specified paths
 * 
 * @param skey pointer, where private key should be stored
 * @param vkey pointer, where public key should be stored
 * @param lanbeacon_keys configuration for file paths and password
 * @return Returns 0 for success, non-0 otherwise
 * 
 */
int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys);

/**
 * @brief Prints a buffer to stdout. Label is optional
 * 
 * @param label Descriptor that will be put with contents
 * @param buff Buffer for printing
 * @param len Length of the buffer
 * 
 */
void print_it(const char* label, const unsigned char* buff, size_t len);

int passwd_callback(char *pcszBuff,int size,int rwflag, void *pPass);

/**
 * @brief Create signature for LAN-Beacon PDU
 * 
 * @param sig Memory pointer for signature
 * @param slen Length of the created signature
 * @param msg LAN-Beacon PDU that should be signed
 * @param qqlen Size of the passed LAN-Beacon PDU
 * @param lanbeacon_keys Configurations of the keys
 * @return Success or error codes
 * 
 */
int signlanbeacon(	unsigned char** sig, size_t* slen, const unsigned char* msg, 
					size_t qqlen, struct open_ssl_keys *lanbeacon_keys);

/**
 * @brief Read stored pem files into memory
 * 
 * @param skey Memory address for the private key
 * @param vkey Memory address for the public key
 * @param lanbeacon_keys Configurations of the keys
 * @return Success or error codes
 * 
 */
int read_keys(EVP_PKEY** skey, EVP_PKEY** vkey, struct open_ssl_keys *lanbeacon_keys);

/**
 * @brief Verify the signature for LAN-Beacon PDUs
 * 
 * @param msg Message, that should be verified
 * @param mlen Length of the message, that should be verified
 * @param lanbeacon_keys Configurations of the keys
 * @return Success or error codes
 * 
 */
int verifylanbeacon(const unsigned char* msg, size_t mlen, 
					struct open_ssl_keys *lanbeacon_keys);

#endif
