/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if.h>
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

#include "rawsocket_LLDP.h"
#include "receiver.h"
#include "openssl_sign.h"
#include "define.h"
#include "openssl_sign.h"
#define _GNU_SOURCE	 /* To get defns of NI_MAXSERV and NI_MAXHOST */

#define LLDP_DEST_MAC0	0x01
#define LLDP_DEST_MAC1	0x80
#define LLDP_DEST_MAC2	0xc2
#define LLDP_DEST_MAC3	0x00
#define LLDP_DEST_MAC4	0x00
#define LLDP_DEST_MAC5	0x0e

#define CHALLENGE_ETHTYPE 0x88B5
#define LLDP_ETHER_TYPE	0x88CC

#define SEND_SOCKET 0
#define REC_SOCKET 1

#define SEND_FREQUENCY 2

#define SET_SELECT_FDS FD_ZERO(&readfds); for (int i = 0; i < numInterfaces; i++) {FD_SET(sockfd[i], &readfds);}

//TODO gettext

void getInterfaces (int *sockfd, int *numInterfaces, unsigned short etherType, unsigned short sendOrReceive, struct ifreq *if_idx, struct ifreq *if_mac) {
	
	struct ifaddrs *interfaces;
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	
	if (sendOrReceive == SEND_SOCKET) {
		for (; interfaces != NULL; interfaces = interfaces->ifa_next) {
		
			if (interfaces->ifa_addr == NULL)	continue;
			if (!(interfaces->ifa_addr->sa_family == AF_PACKET))	continue; 
		
			/* Open RAW socket to send on */
			if ((sockfd[*numInterfaces] = socket(AF_PACKET, SOCK_RAW, htons(etherType))) == -1) {	
				perror("socket");
			}
		
			/* Get the index of the interface to send on */
			memset(&if_idx[*numInterfaces], 0, sizeof(struct ifreq));
			memcpy(if_idx[*numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(sockfd[*numInterfaces], SIOCGIFINDEX, &if_idx[*numInterfaces]) < 0)
				perror("SIOCGIFINDEX");
			/* Get the MAC address of the interface to send on */
			memset(&if_mac[*numInterfaces], 0, sizeof(struct ifreq));
			memcpy(if_mac[*numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(sockfd[*numInterfaces], SIOCGIFHWADDR, &if_mac[*numInterfaces]) < 0)
				perror("SIOCGIFHWADDR");
		
			printf("Number %i is interface %s\n", *numInterfaces, interfaces->ifa_name);
			*numInterfaces = *numInterfaces + 1;
		}
	}
	
	if (sendOrReceive == REC_SOCKET) {
		
	}
	
	
	return;
}



// parts of code based on https://gist.github.com/austinmarton/1922600
int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs, struct open_ssl_keys *lanbeacon_keys)
{
	int sockfd[20];
	struct ifreq if_idx[20];
	struct ifreq if_mac[20];
	int numInterfaces = 0;
	int frameLength = 0;
	char lldpEthernetFrame[LLDP_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) lldpEthernetFrame;
	struct sockaddr_ll socket_address;
	unsigned long *receivedChallenge = calloc(sizeof(unsigned long), 1);
	
	/* Construct the Ethernet header */
	memset(lldpEthernetFrame, 0, LLDP_BUF_SIZ);

	eh->ether_dhost[0] = LLDP_DEST_MAC0;
	eh->ether_dhost[1] = LLDP_DEST_MAC1;
	eh->ether_dhost[2] = LLDP_DEST_MAC2;
	eh->ether_dhost[3] = LLDP_DEST_MAC3;
	eh->ether_dhost[4] = LLDP_DEST_MAC4;
	eh->ether_dhost[5] = LLDP_DEST_MAC5;
	
	/* Ethertype field */
	eh->ether_type = htons(LLDP_ETHER_TYPE);
	frameLength += sizeof(struct ether_header);
	
	/* Packet data */
	memcpy(&lldpEthernetFrame[frameLength], LANbeaconCustomTLVs, LLDPDU_len);
	frameLength += LLDPDU_len; 
	
	/* End of LLDPDU TLV */
	lldpEthernetFrame[frameLength++] = 0x00;
	lldpEthernetFrame[frameLength++] = 0x00;
	
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = LLDP_DEST_MAC0;
	socket_address.sll_addr[1] = LLDP_DEST_MAC1;
	socket_address.sll_addr[2] = LLDP_DEST_MAC2;
	socket_address.sll_addr[3] = LLDP_DEST_MAC3;
	socket_address.sll_addr[4] = LLDP_DEST_MAC4;
	socket_address.sll_addr[5] = LLDP_DEST_MAC5;
	
	/* Get interfaces */

//	getInterfaces (sockfd, &numInterfaces, LLDP_ETHER_TYPE, SEND_SOCKET, if_idx, if_mac);
	
	//////////////////////////////////////////////////TODO HIER INTERFACES

	struct ifaddrs *interfaces;
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	
	for (; interfaces != NULL; interfaces = interfaces->ifa_next) {
		
		if (interfaces->ifa_addr == NULL)	continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET))	continue; 
		
		/* Open RAW socket to send on */
		if ((sockfd[numInterfaces] = socket(AF_PACKET, SOCK_RAW, htons(LLDP_ETHER_TYPE))) == -1) {	//TODO
			perror("socket");
		}
		
		/* Get the index of the interface to send on */
		memset(&if_idx[numInterfaces], 0, sizeof(struct ifreq));
		memcpy(if_idx[numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		if (ioctl(sockfd[numInterfaces], SIOCGIFINDEX, &if_idx[numInterfaces]) < 0)
			perror("SIOCGIFINDEX");
		/* Get the MAC address of the interface to send on */
		memset(&if_mac[numInterfaces], 0, sizeof(struct ifreq));
		memcpy(if_mac[numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		if (ioctl(sockfd[numInterfaces], SIOCGIFHWADDR, &if_mac[numInterfaces]) < 0)
			perror("SIOCGIFHWADDR");
		
		printf("Number %i is interface %s\n", numInterfaces, interfaces->ifa_name);
		
		numInterfaces++;
	}
	
	
	//////////////////////////////////////////////////
	
	while (1) {
	
		// send packets on all interfaces
		for(int i = 0; i < numInterfaces; i++) {

			/* Ethernet header */
			eh->ether_shost[0] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[0];
			eh->ether_shost[1] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[1];
			eh->ether_shost[2] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[2];
			eh->ether_shost[3] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[3];
			eh->ether_shost[4] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[4];
			eh->ether_shost[5] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[5];

			/* Index of the network device */
			socket_address.sll_ifindex = if_idx[i].ifr_ifindex;
			
			/* Send packet */
			if (sendto(sockfd[i], lldpEthernetFrame, frameLength, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
				printf(_("Send failed on interface number %i\n"), i);
			else
				printf(_("Successfully sent on interface number %i\n"), i);
		}
		
		if (*receivedChallenge == 0) {
			*receivedChallenge = receiveChallenge();
			*receivedChallenge = htonl(*receivedChallenge);
			memcpy(&lldpEthernetFrame[frameLength-272+6], receivedChallenge, 4);
		}
		else
			sleep(2);
		
		//## add signature ##//
		time_t timeStamp = time(NULL);
		timeStamp = htonl(timeStamp);
		memcpy(&lldpEthernetFrame[frameLength-272+6+4], &timeStamp, 4);
		unsigned char* sig = NULL;
		size_t slen = 0;
		signLANbeacon(&sig, &slen, (const unsigned char *) &lldpEthernetFrame[14], (size_t) LLDPDU_len - 256, lanbeacon_keys); 
		memcpy(&lldpEthernetFrame[frameLength-272+6+4+4], sig, slen);
		
	}
	
	return EXIT_SUCCESS;
}


// parts of code based on https://gist.github.com/austinmarton/2862515
struct received_lldp_packet *recLLDPrawSock(struct open_ssl_keys *lanbeacon_keys) {
	
	struct received_lldp_packet *my_received_lldp_packet = malloc(sizeof(struct received_lldp_packet));
	struct ether_header *eh = (struct ether_header *) my_received_lldp_packet->lldpReceivedPayload;
	
	int sockfd[20];
	int sockopt[20];
	
	// parameters for select()
	int maxSockFd = 0;
	struct timeval tv;
	fd_set readfds;
	

	
	int numInterfaces = 0;
	struct ifaddrs *interfaces;
	
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	
	for ( ;interfaces != NULL; interfaces = interfaces->ifa_next) {
//TODO printf("%s\n",interfaces->ifa_name);
		
		// Skip if interface is not in use or wrong type
		if (interfaces->ifa_addr == NULL)	continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET))	continue; 
		
		/* Open PF_PACKET socket, listening for EtherType 0x88CC */
		if ((sockfd[numInterfaces] = socket(PF_PACKET, SOCK_RAW, htons(LLDP_ETHER_TYPE))) == -1) {
			perror("listener: socket");	
		}

		/* Set interface to promiscuous mode */
		struct ifreq ifopts;
		memcpy(ifopts.ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		ioctl(sockfd[numInterfaces], SIOCGIFFLAGS, &ifopts);
		ifopts.ifr_flags |= IFF_PROMISC;
		ioctl(sockfd[numInterfaces], SIOCSIFFLAGS, &ifopts);
	
		/* Allow the socket to be reused - incase connection is closed prematurely */
		if (setsockopt(sockfd[numInterfaces], SOL_SOCKET, SO_REUSEADDR, &sockopt[numInterfaces], sizeof sockopt) == -1) {
			perror("setsockopt");
			close(sockfd[numInterfaces]);
			exit(EXIT_FAILURE);
		}
		/* Bind to device */
		if (setsockopt(sockfd[numInterfaces], SOL_SOCKET, SO_BINDTODEVICE, interfaces->ifa_name, IFNAMSIZ-1) == -1)	{
			perror("SO_BINDTODEVICE");
			close(sockfd[numInterfaces]);
			exit(EXIT_FAILURE);
		}
printf("Number %i is interface %s\n", numInterfaces, interfaces->ifa_name);
		
//		FD_SET(sockfd[numInterfaces], &readfds);
		
		if (sockfd[numInterfaces] > maxSockFd)
			maxSockFd = sockfd[numInterfaces];
		
		numInterfaces++;
	}
	/*neu
	for (int i = 0; i < numInterfaces; i++) {
		if (sockfd[i] > maxSockFd)
			maxSockFd = sockfd[i];
	}*/
	
	/////////////////HIER GEHT DER NEUE TEIL LOS
	///////ASGFKHÖSALKHGÖSAGHSGÖSA
	
	int challengeSentBool = 0;
	int rv;
	
/*neu
	// receive one LANbeacon frame, send a challenge to the source MAC and 
	// then wait for the frame containing the challenge
*/	
	// read first packet
	SET_SELECT_FDS
//	tv.tv_sec = 10000;
	rv = select(maxSockFd, &readfds, NULL, NULL, NULL);
puts("neuTest");	
	if (rv == -1) perror("select"); // error occurred in select()
	else if (rv == 0) printf("Timeout occurred! No data after %i seconds.\n", SEND_FREQUENCY);
	else {
		for (int i = 0; i < numInterfaces; i++) {
			if (FD_ISSET(sockfd[i], &readfds)) {
	
				my_received_lldp_packet->payloadSize = recvfrom(sockfd[i], my_received_lldp_packet->lldpReceivedPayload, LLDP_BUF_SIZ, 0, NULL, NULL);

puts("neuTest");	
				// Check if the packet was sent to the LLDP multicast MAC address
				if (!(	eh->ether_dhost[0] == LLDP_DEST_MAC0 &&
						eh->ether_dhost[1] == LLDP_DEST_MAC1 &&
						eh->ether_dhost[2] == LLDP_DEST_MAC2 &&
						eh->ether_dhost[3] == LLDP_DEST_MAC3 &&
						eh->ether_dhost[4] == LLDP_DEST_MAC4 &&
						eh->ether_dhost[5] == LLDP_DEST_MAC5)) {
//*neu DER BLOCK MUSS RAUS					
					//## Verify signature ##//
					if (0 != verifyLANbeacon(&my_received_lldp_packet->lldpReceivedPayload[14], my_received_lldp_packet->payloadSize - 2 - 14, lanbeacon_keys)) {	// - end of LLDPDU 2 - 14 Ethernet header
						puts("problem with verification");
						continue;
					}
					break;
				}
			}
		}
	}
	
	if (0 == challengeSentBool++) {
		// delete received packet, send challenge and flush buffer
		memset (my_received_lldp_packet->lldpReceivedPayload, 0, LLDP_BUF_SIZ);
	
		sendChallenge (eh->ether_shost, 65534);
	
	//	SET_SELECT_FDS
		tv.tv_sec = 0;
		while (1) {
			SET_SELECT_FDS
			rv = select(maxSockFd, &readfds, NULL, NULL, &tv);
	puts("neuneuneu");//sleep(1);
			if (rv == -1) perror("select"); // error occurred in select()
			else if (rv == 0) {
				printf("Timeout occurred! All data flushed.\n");
				break;
			}
			else {
				for (int i = 0; i < numInterfaces; i++) {
					if (FD_ISSET(sockfd[i], &readfds)) {
	puts("Deleted something");
						recvfrom(sockfd[i], NULL, 1500, 0, NULL, NULL);
					}
				}
			}
		}
	}
	
	
	
	// read second packet
	SET_SELECT_FDS
//	tv.tv_sec = 10000;
	rv = select(maxSockFd, &readfds, NULL, NULL, NULL);
puts("neuTest");	
	if (rv == -1) perror("select"); // error occurred in select()
	else if (rv == 0) printf("Timeout occurred! No data after %i seconds.\n", SEND_FREQUENCY);
	else {
		for (int i = 0; i < numInterfaces; i++) {
			if (FD_ISSET(sockfd[i], &readfds)) {
		
				my_received_lldp_packet->payloadSize = recvfrom(sockfd[i], my_received_lldp_packet->lldpReceivedPayload, LLDP_BUF_SIZ, 0, NULL, NULL);

				// Check if the packet was sent to the LLDP multicast MAC address
				if (!(	eh->ether_dhost[0] == LLDP_DEST_MAC0 &&
						eh->ether_dhost[1] == LLDP_DEST_MAC1 &&
						eh->ether_dhost[2] == LLDP_DEST_MAC2 &&
						eh->ether_dhost[3] == LLDP_DEST_MAC3 &&
						eh->ether_dhost[4] == LLDP_DEST_MAC4 &&
						eh->ether_dhost[5] == LLDP_DEST_MAC5)) {
//*neu DER BLOCK MUSS RAUS					
					//## Verify signature ##//
					if (0 != verifyLANbeacon(&my_received_lldp_packet->lldpReceivedPayload[14], my_received_lldp_packet->payloadSize - 2 - 14, lanbeacon_keys)) {	// - end of LLDPDU 2 - 14 Ethernet header
						puts("problem with verification");
						continue;
					}
					break;
				}
			}
		}
	}
	
puts("nach dem zweiten Empfangen"); sleep(3);
	
/*/////////////////////////////////////////////////////
	
	while (1) {
		
		for (int i = 0; i < numInterfaces; i++) 
			recvfrom(sockfd[i], NULL, 1500, 0, NULL, NULL);
		
		rv = select(maxSockFd, &readfds, NULL, NULL, &tv);
		
		if (rv == -1) perror("select"); // error occurred in select()
		else if (rv == 0) printf("Timeout occurred! No data after %i seconds.\n", SEND_FREQUENCY);
		else {
			for (int i = 0; i < numInterfaces; i++) {
				if (FD_ISSET(sockfd[i], &readfds)) {
			
	printf("currently on %i\n",i);
					my_received_lldp_packet->payloadSize = recvfrom(sockfd[i], my_received_lldp_packet->lldpReceivedPayload, LLDP_BUF_SIZ, 0, NULL, NULL);

					// Check if the packet was sent to the LLDP multicast MAC address 
					if (!(eh->ether_dhost[0] == LLDP_DEST_MAC0 &&
							eh->ether_dhost[1] == LLDP_DEST_MAC1 &&
							eh->ether_dhost[2] == LLDP_DEST_MAC2 &&
							eh->ether_dhost[3] == LLDP_DEST_MAC3 &&
							eh->ether_dhost[4] == LLDP_DEST_MAC4 &&
							eh->ether_dhost[5] == LLDP_DEST_MAC5)) {
						continue;
					}
	printf("found on %i\n",i);

					if (challengeSentBool == 0)	{
						sendChallenge (eh->ether_shost, 65534);
						challengeSentBool++;
						recvfrom(sockfd[i], NULL, 1500, 0, NULL, NULL);
						continue;
					}
			
					//## Verify signature ##//
					if (0 != verifyLANbeacon(&my_received_lldp_packet->lldpReceivedPayload[14], my_received_lldp_packet->payloadSize - 2 - 14, lanbeacon_keys)) {	// - end of LLDPDU 2 - 14 Ethernet header
						puts("problem with verification");
						continue;
					}
			
					break;
				}
			}
		}
	}
	
*//////////////////////////////////////////////////////

	for (int i = 0; i < numInterfaces; i++) 
		close(sockfd[i]);
	
	return my_received_lldp_packet;
}


#define CHALLENGE_ETHTYPE 0x88B5

void sendChallenge (unsigned char *destination_mac, unsigned long challenge) {
	
	int challenge_len = 4;
	
//printf("ChallengeSize: %zu\n",sizeof(long));
	
	challenge = htonl(challenge);
	
	int sockfd[20];
	struct ifreq if_idx[20];
	struct ifreq if_mac[20];
	int frameLength = 0;
	char lldpEthernetFrame[LLDP_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) lldpEthernetFrame;
	struct sockaddr_ll socket_address;
	
	/* Construct the Ethernet header */
	memset(lldpEthernetFrame, 0, LLDP_BUF_SIZ);
	
	memcpy(eh->ether_dhost, destination_mac, 6);
	
	/* Ethertype field */
	eh->ether_type = htons(CHALLENGE_ETHTYPE);		// TODO
	frameLength += sizeof(struct ether_header);
	
	/* Packet data */
	memcpy(&lldpEthernetFrame[frameLength], &challenge, challenge_len);
	frameLength += challenge_len; 
	
//	/* End of LLDPDU TLV */
//	lldpEthernetFrame[frameLength++] = 0x00;
//	lldpEthernetFrame[frameLength++] = 0x00;
	
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	memcpy(socket_address.sll_addr, destination_mac, 6);
	
	/* Get interfaces */
	struct ifaddrs *interfaces;
	int numInterfaces = 0;
	
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	
	for (; interfaces != NULL; interfaces = interfaces->ifa_next) {
		
		if (interfaces->ifa_addr == NULL)	continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET))	continue; 
		
		/* Open RAW socket to send on */
		if ((sockfd[numInterfaces] = socket(AF_PACKET, SOCK_RAW, htons(CHALLENGE_ETHTYPE))) == -1) {	//TODO
			perror("socket");
		}
		
		/* Get the index of the interface to send on */
		memset(&if_idx[numInterfaces], 0, sizeof(struct ifreq));
		memcpy(if_idx[numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		if (ioctl(sockfd[numInterfaces], SIOCGIFINDEX, &if_idx[numInterfaces]) < 0)
			perror("SIOCGIFINDEX");
		/* Get the MAC address of the interface to send on */
		memset(&if_mac[numInterfaces], 0, sizeof(struct ifreq));
		memcpy(if_mac[numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		if (ioctl(sockfd[numInterfaces], SIOCGIFHWADDR, &if_mac[numInterfaces]) < 0)
			perror("SIOCGIFHWADDR");
		
		printf("Number %i is interface %s\n", numInterfaces, interfaces->ifa_name);
		
		numInterfaces++;
	}
	
	for (int i = 0; i<2; i++) {
		sleep(1);
		
//		struct timespec ts;
//		ts.tv_nsec = 200000000L;

//		nanosleep(&ts, null);
		
		// send packets on all interfaces
		for(int i = 0; i < numInterfaces; i++) {

			/* Ethernet header */
			eh->ether_shost[0] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[0];
			eh->ether_shost[1] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[1];
			eh->ether_shost[2] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[2];
			eh->ether_shost[3] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[3];
			eh->ether_shost[4] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[4];
			eh->ether_shost[5] = ((uint8_t *)&if_mac[i].ifr_hwaddr.sa_data)[5];

			/* Index of the network device */
			socket_address.sll_ifindex = if_idx[i].ifr_ifindex;
			
			/* Send packet */
			if (sendto(sockfd[i], lldpEthernetFrame, frameLength, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
				printf("Send failed on interface number %i\n", i);
			else
				printf("Successfully sent CHALLENGE on interface number %i\n", i);
			
		}
		
		sleep(1);
		
	}
	
	return;
}


// parts of code based on https://gist.github.com/austinmarton/2862515
unsigned long receiveChallenge() {
	
	unsigned char *receiveBuf = calloc(300, 1);
	int receivedSize;
	unsigned long *receivedChallenge = calloc(sizeof(unsigned long), 1);
	
	struct ether_header *eh = (struct ether_header *) receiveBuf;
	int sockfd[20];
	int sockopt[20];
	
	// parameters for select()
	int maxSockFd = 0;
	struct timeval tv;
	tv.tv_sec = SEND_FREQUENCY;
	fd_set readfds;
	FD_ZERO(&readfds);
	
	int numInterfaces = 0;
	struct ifaddrs *interfaces;
	
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	
	for ( ;interfaces != NULL; interfaces = interfaces->ifa_next) {
//TODO printf("%s\n",interfaces->ifa_name);
		
		// Skip if interface is not in use or wrong type
		if (interfaces->ifa_addr == NULL)	continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET))	continue; 
		
		/* Open PF_PACKET socket, listening for EtherType 0x88CC */
		if ((sockfd[numInterfaces] = socket(PF_PACKET, SOCK_RAW, htons(CHALLENGE_ETHTYPE))) == -1) {
			perror("listener: socket");	
		}

		/* Set interface to promiscuous mode */
		struct ifreq ifopts;
		memcpy(ifopts.ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
		ioctl(sockfd[numInterfaces], SIOCGIFFLAGS, &ifopts);
		ifopts.ifr_flags |= IFF_PROMISC;
		ioctl(sockfd[numInterfaces], SIOCSIFFLAGS, &ifopts);
	
		/* Allow the socket to be reused - incase connection is closed prematurely */
		if (setsockopt(sockfd[numInterfaces], SOL_SOCKET, SO_REUSEADDR, &sockopt[numInterfaces], sizeof sockopt) == -1) {
			perror("setsockopt");
			close(sockfd[numInterfaces]);
			exit(EXIT_FAILURE);
		}
		/* Bind to device */
		if (setsockopt(sockfd[numInterfaces], SOL_SOCKET, SO_BINDTODEVICE, interfaces->ifa_name, IFNAMSIZ-1) == -1)	{
			perror("SO_BINDTODEVICE");
			close(sockfd[numInterfaces]);
			exit(EXIT_FAILURE);
		}
printf("Number %i is interface %s\n", numInterfaces, interfaces->ifa_name);
		
		FD_SET(sockfd[numInterfaces], &readfds);
		
		numInterfaces++;
	}
	
	for (int i = 0; i < numInterfaces; i++) {
		if (sockfd[i] > maxSockFd)
			maxSockFd = sockfd[i];
	}
	
	int rv = select(maxSockFd, &readfds, NULL, NULL, &tv);
puts("baba");
	if (rv == -1) perror("select"); // error occurred in select()
	else if (rv == 0) printf("Timeout occurred! No data after %i seconds.\n", SEND_FREQUENCY);
	else {
		for (int i = 0; i < numInterfaces; i++) {
			if (FD_ISSET(sockfd[i], &readfds)) {
				
printf("currently on %i\n",i);
				receivedSize = recvfrom(sockfd[i], receiveBuf, 300, 0, NULL, NULL);

//				/* Check if the packet was sent to the LLDP multicast MAC address */
//				if (!(eh->ether_dhost[0] == LLDP_DEST_MAC0 &&
//						eh->ether_dhost[1] == LLDP_DEST_MAC1 &&
//						eh->ether_dhost[2] == LLDP_DEST_MAC2 &&
//						eh->ether_dhost[3] == LLDP_DEST_MAC3 &&
//						eh->ether_dhost[4] == LLDP_DEST_MAC4 &&
//						eh->ether_dhost[5] == LLDP_DEST_MAC5)) {
//					continue;
//				}
printf("found on %i\n",i);
				
				memcpy(receivedChallenge, &receiveBuf[14], 4);
				*receivedChallenge = ntohl(*receivedChallenge);
				printf("Empfangene challenge: %lu\n", *receivedChallenge);
				
				break;
			}
		}
	}
	
	for (int i = 0; i < numInterfaces; i++) 
		close(sockfd[i]);
	
	return *receivedChallenge;
}
