/**
 * @file rawsocket_LAN_Beacon.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief raw-socket sending and receiving
 * 
 */

#ifndef RAWSOCKET_LAN_BEACON_H
#define RAWSOCKET_LAN_BEACON_H

#include <sys/ioctl.h>
#include <net/if.h>
#include "openssl_sign.h"
#include "receiver.h"
#include "sender.h"

#define SEND_SOCKET 0
#define REC_SOCKET 1

/**
 * @brief Receives LAN-Beacons and adds them to the structure of received beacons
 * 
 * @param my_receiver_information Receiver configuration and structs for storing the received beacons
 * 
 */
void new_lan_beacon_receiver (struct receiver_information *my_receiver_information);

/**
 * @brief Shortcut that can be used for sending LAN-Beacons, provides some configuration already
 * 
 * @param my_sender_information Struct that contains everything needed for sending
 * @return Success or failure code, which is passed on from called function
 * 
 */
int send_lan_beacon_rawSock (struct sender_information *my_sender_information);

/**
 * @brief Listen for any and eventually receive challenges, that clients have sent in response to LAN-Beacon frames
 * 
 * @param my_challenge_interfaces Struct with the sockets for receiving challenges
 * @param challenge_dest_mac States the destination to send the authenticated LAN-Beacon
 * @param my_sender_information Sender configurations
 * @return Returnes the value of the received challenge
 * 
 */
unsigned long receiveChallenge(struct interfaces *my_challenge_interfaces, 
				char *challenge_dest_mac, struct sender_information *my_sender_information);

/**
 * @brief Get raw sockets for interfaces
 * 
 * @param my_interfaces_struct Struct that contains interfaces information and configuration
 * @param interface_to_send_on Specified interfaces for sending
 * 
 */
void getInterfaces (struct interfaces *my_interfaces_struct, char *interface_to_send_on);

/**
 * @brief Generic function to send on raw sockets, both handles sending of LAN-Beacon and challenges
 * 
 * @param destination_mac Destination MAC address
 * @param payload Payload that should be sent
 * @param payloadLen Length of payload
 * @param etherType EtherType of payload
 * @param my_sender_information Sender configurations
 * 
 */
void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct sender_information *my_sender_information);

#endif
