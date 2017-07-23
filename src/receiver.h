/**
 * @file receiver.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief Receiver-specific functions and structures
 * 
 */

#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H
#include "define.h"
#include "openssl_sign.h"
#include "rawsocket_LAN_Beacon.h"

/**
 * @brief Contains all the information related to one received frame.
 */
struct received_lan_beacon_frame {
	unsigned char lan_beacon_ReceivedPayload[LAN_BEACON_BUF_SIZ];	/**< Contains the raw received payload from a LAN-Beacon frame. */
	ssize_t payloadSize;		/**< The size of the raw payload. */
	unsigned long challenge;	/**< The challange, that has been sent to the server. */
	unsigned char current_destination_mac[6];	/**< The MAC address of the server, which the frame was received from. */
	
	int successfullyAuthenticated;	/**< Has frame already been authenticated? */
	
	int times_left_to_display;	/**< Countdown, how many more times the frame will be displayed. Is updated, if frame with same content is received again. */
	
	char ** parsedBeaconContents;	/**< Contains the parsed contents, that will be used to print something to the display. */
};

/**
 * @brief Contains all variables, that are needed to access sockets on interfaces.
 */
struct interfaces {
	int sockfd[20];	/**< File descriptors of raw sockets. */
	int sockopt[20];	/**< . Options for each raw socket. */
	int maxSockFd;	/**< Needed for select function. */
	int numInterfaces;	/**< Number of used interfaces. */
	struct ifreq if_idx[20];	/**< Interface IDs. */
	struct ifreq if_mac[20];	/**< Interface MACs. */
	unsigned short etherType;	/**< EtherType to send or receive on interface. */
	unsigned short sendOrReceive;	/**< Switch for send or receive mode. */
};

/**
 * @brief Receiver configurations. 
 */
struct receiver_information {
	
	int authenticated_mode;	/**< Has user specified using the authenticated mode? */
	int scroll_speed;	/**< How fast the display should switch to the next display page. */
	int current_lan_beacon_pdu_for_printing;	/**< The currently printed PDU. */
	
	struct received_lan_beacon_frame *pointers_to_received_frames[20];	/**< Frames, that currently are stored for displaying. */
	int number_of_currently_received_frames;	/**< How many frames are currently stored for displaying. */
	
	struct open_ssl_keys lanbeacon_keys;	/**< The paths to the keys. */
	struct interfaces my_receiver_interfaces;	/**< Interfaces, that are used for LAN-Beacon reception. */
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
int receiver(int argc, char **argv);

/**
 * @brief This function takes raw received LAN-Beacon frames and creates strings from them, that can be used for printing or further processing
 * 
 * @param my_received_lan_beacon_frame Pointer to one single received LAN-Beacon frame, that should be evaluated
 * @param lanbeacon_keys Pointer to struct for keys, needed in order to verify authentication information
 * @return Returns parsed content as an array of TLV-descriptor and TLV-content pairs
 * 
 */
char ** evaluatelanbeacon (struct received_lan_beacon_frame *my_received_lan_beacon_frame, struct open_ssl_keys *lanbeacon_keys);


/**
 * @brief This function prints the received content on the standard output and, if compiler flags are set, also on a C-Berry display
 * 
 * @param my_receiver_information receiver information struct, that contains display settings and contents that should be printed
 * 
 */
void bananaPIprint (struct receiver_information *my_receiver_information);

#endif

