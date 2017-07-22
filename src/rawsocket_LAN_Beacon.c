#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
//#include <linux/if.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <openssl/evp.h>
#include <sys/select.h>
#include <libintl.h>
#include <locale.h>

#include "rawsocket_LAN_Beacon.h"
#include "receiver.h"
#include "openssl_sign.h"
#include "define.h"
#include "openssl_sign.h"
#include "sender.h"
#define _GNU_SOURCE

// parts of code based on https://gist.github.com/austinmarton/1922600
void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct open_ssl_keys *lanbeacon_keys, 
					char *interface_to_send_on, struct sender_information *my_sender_information) {
	
	if (etherType == CHALLENGE_ETHTYPE)
		* (unsigned long *) payload = htonl(*(unsigned long *) payload);
	int frameLength = 0;
	char lan_beacon_EthernetFrame[LAN_BEACON_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) lan_beacon_EthernetFrame;
	struct sockaddr_ll socket_address;

	unsigned long *receivedChallenge = NULL;

	if (etherType == LAN_BEACON_ETHER_TYPE) {
		receivedChallenge = calloc(sizeof(unsigned long), 1);
		if(!receivedChallenge) 
			puts(_("calloc error of \"receivedChallenge\" in sendRawSocket"));
	}

	// Construct the Ethernet header
	memset(lan_beacon_EthernetFrame, 0, LAN_BEACON_BUF_SIZ);
	memcpy(eh->ether_dhost, destination_mac, 6);

	// Ethertype field
	eh->ether_type = htons(etherType);
	frameLength += sizeof(struct ether_header);

	// Packet data
	memcpy(&lan_beacon_EthernetFrame[frameLength], payload, payloadLen);
	frameLength += payloadLen;
	
	// Address length and destination MAC
	socket_address.sll_halen = ETH_ALEN;
	memcpy(socket_address.sll_addr, destination_mac, 6);

	// Get interfaces
	struct interfaces my_interfaces = {
		.numInterfaces = 0,
		.etherType = etherType,
		.sendOrReceive = SEND_SOCKET,
//		.maxSockFd = NULL,
//		.sockopt = NULL
	};
	
//	int sockfd[20];
//	int numInterfaces = 0;
//	struct ifreq if_idx[20];
//	struct ifreq if_mac[20];
	getInterfaces (&my_interfaces, interface_to_send_on);
	
	struct interfaces my_challenge_interfaces = {
		.numInterfaces = 0,
		.maxSockFd = 0,
		.etherType = CHALLENGE_ETHTYPE,
		.sendOrReceive = REC_SOCKET,
//		.if_idx = NULL,
//		.if_mac = NULL
	};
	
	getInterfaces (&my_challenge_interfaces, NULL);

	while (1) {
		// send frames on all interfaces
		for(int j = 0; j < my_interfaces.numInterfaces; j++) {

			// Ethernet header, destination MAC address
			memcpy(eh->ether_shost, ((uint8_t *)&my_interfaces.if_mac[j].ifr_hwaddr.sa_data), 6);
			
			if (etherType == LAN_BEACON_ETHER_TYPE) {
				// Port and chassis subtype TLVs filled
				memcpy(&lan_beacon_EthernetFrame[17], ((uint8_t *)&my_interfaces.if_mac[j].ifr_hwaddr.sa_data), 6);
				memcpy(&lan_beacon_EthernetFrame[26], ((uint8_t *)&my_interfaces.if_mac[j].ifr_hwaddr.sa_data), 6);
				
				if (*receivedChallenge != 0) {
					
					unsigned char* sig = NULL;
					size_t slen = 0;
					
					// - 2 End of LAN-Beacon PDU TLV + 2 Auth TLV header + 4 OUI/subtype + 4 challenge + 4 timestamp
					signlanbeacon(&sig, &slen, (const unsigned char *) &lan_beacon_EthernetFrame[14], 
						(size_t) payloadLen - 2 + 2 + 4 + 4 + 4, lanbeacon_keys);
					
					memcpy(&lan_beacon_EthernetFrame[frameLength-2-264+4+4], sig, slen);
					free(sig);
				}
				
			}
		
			// Index of the network device
			socket_address.sll_ifindex = my_interfaces.if_idx[j].ifr_ifindex;

			// Send frame
			if (sendto(my_interfaces.sockfd[j], lan_beacon_EthernetFrame, frameLength, 0, 
				(struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
					printf(_("Send failed on interface number %i\n"), j);
			else
				printf(_("Successfully sent on interface number %i\n"), j);
		}

		if (etherType == CHALLENGE_ETHTYPE) {
			// wait for server to respond
			sleep(1);
			break;
		}

		if (etherType == LAN_BEACON_ETHER_TYPE) {
			
			if (*receivedChallenge != 0) {
			
				*receivedChallenge = 0;
			
				memcpy(eh->ether_dhost, destination_mac, 6);
				memcpy(socket_address.sll_addr, destination_mac, 6);
			
				frameLength -= 270;
				lan_beacon_EthernetFrame[frameLength-2] = 0x00;
				lan_beacon_EthernetFrame[frameLength-1] = 0x00;
			}
			
			if (*receivedChallenge == 0) {

				// receive challenge
				char challenge_dest_mac[6];
				*receivedChallenge = receiveChallenge(	&my_challenge_interfaces,
														challenge_dest_mac,
														my_sender_information);
				*receivedChallenge = htonl(*receivedChallenge);
				
				if (*receivedChallenge != 0) {
					memcpy(eh->ether_dhost, challenge_dest_mac, 6);
					memcpy(socket_address.sll_addr, challenge_dest_mac, 6);
				
					// generate timestamp
					time_t timeStamp = time(NULL);
					timeStamp = htonl(timeStamp);
				
					// add signature TLV
				
					// LAN-Beacon PDU trailing zeros removed
					frameLength -= 2;
					
					char signaturePlaceholder[265];
					memset(signaturePlaceholder, '#', 264);
					signaturePlaceholder[264] = 0;
					
					transferToCombinedBeacon (SUBTYPE_SIGNATURE, signaturePlaceholder, lan_beacon_EthernetFrame, &frameLength, 264);
				
					memcpy(&lan_beacon_EthernetFrame[frameLength-264], receivedChallenge, 4);
				
					memcpy(&lan_beacon_EthernetFrame[frameLength-264+4], &timeStamp, 4);
				
				
					unsigned char* sig = NULL;
					size_t slen = 0;
					
					// 14 = Size of Ethernet header
					signlanbeacon(&sig, &slen, (const unsigned char *) &lan_beacon_EthernetFrame[14], 
						(size_t) payloadLen + 4 + 4 + 4, lanbeacon_keys);
					
					memcpy(&lan_beacon_EthernetFrame[frameLength-264+4+4], sig, slen);
					free(sig);
				
				
					lan_beacon_EthernetFrame[frameLength++] = 0x00;
					lan_beacon_EthernetFrame[frameLength++] = 0x00;
				
				}
				
			}
			else
				sleep(my_sender_information->send_frequency);
		}
	}
	return;
}


int send_lan_beacon_rawSock (struct sender_information *my_sender_information)
{
	sendRawSocket ((unsigned char[6]){LAN_BEACON_DEST_MAC}, my_sender_information->lanBeacon_PDU, 
		my_sender_information->lan_beacon_pdu_len, LAN_BEACON_ETHER_TYPE, &my_sender_information->lanbeacon_keys, 
		my_sender_information->interface_to_send_on, my_sender_information);
	return EXIT_SUCCESS;
}


// parts of code based on https://gist.github.com/austinmarton/2862515
void new_lan_beacon_receiver (struct receiver_information *my_receiver_information) {
	
	unsigned char lan_beacon_receiveBuffer[LAN_BEACON_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) lan_beacon_receiveBuffer;
	
	struct received_lan_beacon_frame *my_received_lan_beacon_frame;// = 

	int receiveBufferSize = 0;
	// parameters for select()
	struct timeval tv = {0, 0};	
	fd_set readfds;
	int rv;
	
	int number_of_bytes_to_compare_for_equal_check;
	int iterator_current_frame_in_received_frames_array;
	
	while(1) {
	
		FD_ZERO(&readfds); 
		for (int x = 0; x < my_receiver_information->my_receiver_interfaces.numInterfaces; x++) 
			FD_SET(my_receiver_information->my_receiver_interfaces.sockfd[x], &readfds);
	
		rv = select(my_receiver_information->my_receiver_interfaces.maxSockFd, &readfds, NULL, NULL, 
					my_receiver_information->number_of_currently_received_frames ? &tv : NULL);

		if (rv == -1) 
			perror("select");
		else if (rv == 0) {
			printf("All current data received.\n");
			break;
		}
		else {
			for (int i = 0; i < my_receiver_information->my_receiver_interfaces.numInterfaces; i++) {
				if (FD_ISSET(my_receiver_information->my_receiver_interfaces.sockfd[i], &readfds)) {

					receiveBufferSize = 
						recvfrom(my_receiver_information->my_receiver_interfaces.sockfd[i], lan_beacon_receiveBuffer, 
							LAN_BEACON_BUF_SIZ, 0, NULL, NULL);
					
					// if frame has been sent to broadcast, it is not authenticated
					// if it is sent to singlecast, it is authenticated
					number_of_bytes_to_compare_for_equal_check = 
						memcmp((unsigned char[6]){LAN_BEACON_DEST_MAC}, eh->ether_dhost, 6) ? 
						receiveBufferSize - 272 : receiveBufferSize - 2;
					
					
					for (iterator_current_frame_in_received_frames_array = 0; 
							iterator_current_frame_in_received_frames_array < my_receiver_information->number_of_currently_received_frames; 
							iterator_current_frame_in_received_frames_array++) {
						if (0 == memcmp(&lan_beacon_receiveBuffer[14], 
										&my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->lan_beacon_ReceivedPayload[14],
										number_of_bytes_to_compare_for_equal_check - 14)) {
							
							// if no authentication is required, just reset display countdown
							if (!my_receiver_information->authenticated_mode) {
								my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->times_left_to_display = SHOW_FRAMES_X_TIMES;
								break;
							}
							
							// if frame has been authenticated already, just reset display countdown
							if (my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->successfullyAuthenticated) {
								my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->times_left_to_display = SHOW_FRAMES_X_TIMES;
								break;
							}
							
							// if another unauthenticated broadcast frame 
							// with same contents as before arrives
							// but an authenticated one is expected, break
							if (my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->payloadSize == receiveBufferSize) {
								break;
							}
							
							// if none of the earlier conditions apply,  
							// - authentication mode is active
							// - no authenticated version has been received before
							// - the frame has authentication information
							memcpy(my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->lan_beacon_ReceivedPayload, lan_beacon_receiveBuffer, receiveBufferSize);
							my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->payloadSize = receiveBufferSize;
							memcpy(my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->current_destination_mac, eh->ether_shost, 6);
							my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->times_left_to_display = SHOW_FRAMES_X_TIMES;
							my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array]->parsedBeaconContents 
								= evaluatelanbeacon(my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array], 
									&my_receiver_information->lanbeacon_keys);							
							break; 
						}
					}
					
					// if a frame with the same contents hasn't been received before,
					// add it to the list of frames
					if (iterator_current_frame_in_received_frames_array == my_receiver_information->number_of_currently_received_frames) {
						
						
						struct received_lan_beacon_frame *my_received_lan_beacon_frame = 
							malloc(sizeof(struct received_lan_beacon_frame));
						if(!my_received_lan_beacon_frame) 
							puts(_("malloc error of \"my_received_lan_beacon_frame\" in new_lan_beacon_receiver"));						
						
						memcpy(my_received_lan_beacon_frame->lan_beacon_ReceivedPayload, lan_beacon_receiveBuffer, receiveBufferSize);
						my_received_lan_beacon_frame->payloadSize = receiveBufferSize;
						memcpy(my_received_lan_beacon_frame->current_destination_mac, eh->ether_shost, 6);
						my_received_lan_beacon_frame->times_left_to_display = SHOW_FRAMES_X_TIMES;
						my_received_lan_beacon_frame->successfullyAuthenticated = 0;
						my_received_lan_beacon_frame->parsedBeaconContents 
							= evaluatelanbeacon(my_received_lan_beacon_frame, &my_receiver_information->lanbeacon_keys);
						
						if (my_receiver_information->authenticated_mode) {
							srand(time(NULL));

							my_received_lan_beacon_frame->challenge = 1+ (rand() % 4294967294);

							sendRawSocket (my_received_lan_beacon_frame->current_destination_mac, 
									&my_received_lan_beacon_frame->challenge, 
									4, CHALLENGE_ETHTYPE, NULL, NULL, NULL);
						}
						
						my_receiver_information->pointers_to_received_frames[iterator_current_frame_in_received_frames_array] = my_received_lan_beacon_frame;
						my_receiver_information->number_of_currently_received_frames++;
					}
				}
			}
		}
	}
}


// parts of code based on https://gist.github.com/austinmarton/2862515
unsigned long receiveChallenge(struct interfaces *my_challenge_interfaces, 
			char *challenge_dest_mac, struct sender_information *my_sender_information) {

	unsigned char *receiveBuf = calloc(300, 1);
	if(!receiveBuf) puts(_("calloc error of \"receiveBuf\" in receiveChallenge"));
	int receivedSize;
	unsigned long *receivedChallenge = calloc(sizeof(unsigned long), 1);
	if(!receivedChallenge) puts(_("calloc error of \"receivedChallenge\" in receiveChallenge"));

	struct ether_header *eh = (struct ether_header *) receiveBuf;

	// parameters for select()
	struct timeval tv = {1, 0};
	tv.tv_sec = my_sender_information->send_frequency;
	fd_set readfds;

	FD_ZERO(&readfds); 
	for (int x = 0; x < my_challenge_interfaces->numInterfaces; x++) {
		FD_SET(my_challenge_interfaces->sockfd[x], &readfds);	

	} 
	int rv = select(my_challenge_interfaces->maxSockFd, &readfds, NULL, NULL, &tv);

	if (rv == -1) 
		perror("select"); // error occurred in select()
	else if (rv == 0) 
		printf(_("Timeout occurred! No data after %i seconds.\n"), my_sender_information->send_frequency);
	else {
		for (int i = 0; i < my_challenge_interfaces->numInterfaces; i++) {
			if (FD_ISSET(my_challenge_interfaces->sockfd[i], &readfds)) {

				receivedSize = recvfrom(my_challenge_interfaces->sockfd[i], receiveBuf, 300, 0, NULL, NULL);

				memcpy(receivedChallenge, &receiveBuf[14], 4);
				*receivedChallenge = ntohl(*receivedChallenge);
				printf(_("Received challenge: %lu\n"), *receivedChallenge);
				memcpy(challenge_dest_mac, &receiveBuf[6], 6);

				break;
			}
		}
	}

	free(receiveBuf);

	return *receivedChallenge;
}


void getInterfaces (struct interfaces *my_interfaces_struct, char *interface_to_send_on) {

	struct ifaddrs *interfaces;
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for (; interfaces != NULL; interfaces = interfaces->ifa_next) {

		
		if (interface_to_send_on) {
			if (strcmp(interface_to_send_on, interfaces->ifa_name) ) {
				continue;
			}
		}

		if (interfaces->ifa_addr == NULL) continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET)) continue;

		// Open RAW socket to send on
		if ((my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces] = socket(PF_PACKET, SOCK_RAW, htons(my_interfaces_struct->etherType))) == -1) {
			perror("socket");
		}

		if (my_interfaces_struct->sendOrReceive == SEND_SOCKET) {

			// Get the index of the interface to send on
			memset(&my_interfaces_struct->if_idx[my_interfaces_struct->numInterfaces], 0, sizeof(struct ifreq));
			memcpy(my_interfaces_struct->if_idx[my_interfaces_struct->numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SIOCGIFINDEX, &my_interfaces_struct->if_idx[my_interfaces_struct->numInterfaces]) < 0)
				perror("SIOCGIFINDEX");
			// Get the MAC address of the interface to send on
			memset(&my_interfaces_struct->if_mac[my_interfaces_struct->numInterfaces], 0, sizeof(struct ifreq));
			memcpy(my_interfaces_struct->if_mac[my_interfaces_struct->numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SIOCGIFHWADDR, &my_interfaces_struct->if_mac[my_interfaces_struct->numInterfaces]) < 0)
				perror("SIOCGIFHWADDR");
		}

		if (my_interfaces_struct->sendOrReceive == REC_SOCKET) {

			// Set interface to promiscuous mode
			struct ifreq ifopts;
			memcpy(ifopts.ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			ioctl(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SIOCGIFFLAGS, &ifopts);
			ifopts.ifr_flags |= IFF_PROMISC;
			ioctl(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SIOCSIFFLAGS, &ifopts);

			// Allow the socket to be reused - incase connection is closed prematurely
			if (setsockopt(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SOL_SOCKET, SO_REUSEADDR, 
				&my_interfaces_struct->sockopt[my_interfaces_struct->numInterfaces], sizeof my_interfaces_struct->sockopt) == -1) {
					perror("setsockopt");
					close(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces]);
					exit(EXIT_FAILURE);
			}

			// Bind to device
			if (setsockopt(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces], SOL_SOCKET, SO_BINDTODEVICE, 
				interfaces->ifa_name, IFNAMSIZ-1) == -1)	{
					perror("SO_BINDTODEVICE");
					close(my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces]);
					exit(EXIT_FAILURE);
			}

			if (my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces] > my_interfaces_struct->maxSockFd)
				my_interfaces_struct->maxSockFd = my_interfaces_struct->sockfd[my_interfaces_struct->numInterfaces];
		}

		printf(_("Number %i is interface %s\n"), my_interfaces_struct->numInterfaces, interfaces->ifa_name);
		my_interfaces_struct->numInterfaces += 1;
	}

	return;
}

