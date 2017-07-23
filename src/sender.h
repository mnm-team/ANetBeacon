/**
 * @file sender.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief Sender-specific functions and structures
 * 
 */

#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H
#include "define.h"
#include "openssl_sign.h"
#include "receiver.h"


/**
 * @brief Sender configurations.
 */
struct sender_information {
	
	char *lanBeacon_PDU;	/**< The combinded payload of a PDU, that is being sent. */
	int lan_beacon_pdu_len;	/**< Length of the combined PDU. */
	int send_frequency;	/**< Number of seconds between each sent PDU. */
	char *interface_to_send_on;	/**< If specified by start parameters, interface that is used for sending. */
	struct interfaces my_challenge_receiver_interfaces;	/**< Interfaces that are used for receiving challenges. */
	struct open_ssl_keys lanbeacon_keys;	/**< Keys configuration. */
};

/**
 * @brief This function has the main receiver logic and starts all other receiver functions
 * 
 * @param argc Number of command line arguments.
 * @param argv Contents of command line arguments.
 * 
 * @return Error or failure code
 * 
 */
int sender(int argc, char **argv);

/**
 * @brief Creates a LAN-Beacon PDU from the command line arguments
 * 
 * Howto for adding new fields:
 * 1. Add defines for desired new field in define.h
 * 2. Add desired options in mergedlanbeaconCreator()
 * 
 * @param argc Number of command line arguments.
 * @param argv Contents of command line arguments.
 * @return Returns an array, that contains the payload of a lanBeacon_PDU
 * 
 */
char *mergedlanbeaconCreator (int *argc, char **argv, struct sender_information *my_sender_information);

/**
 * @brief Shortcut function for cases in which only a string is transferred, no binary format TLVs.
 * 
 * @param subtype Subtype of the TLV
 * @param TLVdescription Descriptor string of the TLV
 * @param combinedString Pointer to the string, that contains text representation of all contents
 * @param source String contents, that should be included to the PDU
 * @param combinedBeacon PDU of beacon, that TLVs should be added to
 * @param currentByte current position in the Beacon-PDU
 * 
 */
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, 
										char **combinedString, char *source, 
										char *combinedBeacon, int *currentByte);

/**
 * @brief Transferring the content of the field to the combined lanbeacon in binary format
 * 
 * @param subtype Subtype of the TLV
 * @param source String contents, that should be included to the PDU
 * @param combinedBeacon PDU of beacon, that TLVs should be added to
 * @param currentByte current position in the Beacon-PDU
 * @param currentTLVlength Length of the passed TLV
 * 
 */
void transferToCombinedBeacon (unsigned char subtype, void *source, char *combinedBeacon, 
								int *currentByte, unsigned short int currentTLVlength);


/**
 * @brief Transfer human-readable information to combined string
 * 
 * Transferring the content of the field to the combined string in human-readable format. If one combined string exceeds 507 byte limit of TLV it is put to the next combined string TLV
 * 
 * @param TLVdescription Descriptor string of the TLV
 * @param combinedString Pointer to the string, that contains text representation of all contents
 * @param source String contents, that should be included to the PDU
 * 
 */
void transferToCombinedString (char *TLVdescription, char **combinedString, char *source);

/**
 * @brief Parse IPv4 or IPv6 subnets to binary format
 * 
 * Using regex to get IP-addresses from string input, then convert them to binary representation for transport
 * 
 * @param ip_V4or6 Switch between IPv4 and IPv6 mode
 * @param optarg String, which should be parsed
 * @param combinedString Pointer to the string, that contains text representation of all contents
 * @param combinedBeacon PDU of beacon, that TLVs should be added to
 * @param currentByte current position in the Beacon-PDU
 * 
 */
void ipParser (int ip_V4or6, char *optarg, char **combinedString, 
				char *combinedBeacon, int *currentByte);

#endif
