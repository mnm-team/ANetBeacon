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
#include "define.h"
#define _GNU_SOURCE	 /* To get defns of NI_MAXSERV and NI_MAXHOST */

#define LLDP_DEST_MAC0	0x01
#define LLDP_DEST_MAC1	0x80
#define LLDP_DEST_MAC2	0xc2
#define LLDP_DEST_MAC3	0x00
#define LLDP_DEST_MAC4	0x00
#define LLDP_DEST_MAC5	0x0e

#define LLDP_ETHER_TYPE	0x88CC

// parts of code based on https://gist.github.com/austinmarton/1922600
int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs)
{
	int sockfd[20];
	struct ifreq if_idx[20];
	struct ifreq if_mac[20];
	int frameLength = 0;
	char LLDPethernetFrame[LLDP_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) LLDPethernetFrame;
	struct sockaddr_ll socket_address;
	
	/* Construct the Ethernet header */
	memset(LLDPethernetFrame, 0, LLDP_BUF_SIZ);

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
	memcpy(&LLDPethernetFrame[frameLength], LANbeaconCustomTLVs, LLDPDU_len);
	frameLength += LLDPDU_len; 
	
	/* End of LLDPDU TLV */
	LLDPethernetFrame[frameLength++] = 0x00;
	LLDPethernetFrame[frameLength++] = 0x00;
	
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
		if ((sockfd[numInterfaces] = socket(AF_PACKET, SOCK_RAW, htons(LLDP_ETHER_TYPE))) == -1) {	
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
	
	while (1) {
		sleep(1);
		
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
			if (sendto(sockfd[i], LLDPethernetFrame, frameLength, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
				printf("Send failed on interface number %i\n", i);
			else
				printf("Successfully sent on interface number %i\n", i);
		}
	}
	
	return 0;
}



// parts of code based on https://gist.github.com/austinmarton/2862515
void recLLDPrawSock(unsigned char *LLDPreceivedPayload, ssize_t *payloadSize)
{
	struct ether_header *eh = (struct ether_header *) LLDPreceivedPayload;
	
	int sockfd[20];
	int sockopt[20];
	
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
		numInterfaces++;
	}
	
	while (1) {
		for (int i = 0; i < numInterfaces; i++) {
	//		printf("listener: Waiting to recvfrom...\n");
			*payloadSize = recvfrom(sockfd[i], LLDPreceivedPayload, LLDP_BUF_SIZ, 0, NULL, NULL);
	//		printf("listener: got packet %lu bytes\n", *payloadSize);
printf("currently on %i\n",i);

			/* Check if the packet was sent to the LLDP multicast MAC address */
			if (!(eh->ether_dhost[0] == LLDP_DEST_MAC0 &&
					eh->ether_dhost[1] == LLDP_DEST_MAC1 &&
					eh->ether_dhost[2] == LLDP_DEST_MAC2 &&
					eh->ether_dhost[3] == LLDP_DEST_MAC3 &&
					eh->ether_dhost[4] == LLDP_DEST_MAC4 &&
					eh->ether_dhost[5] == LLDP_DEST_MAC5)) {
				continue;
			}
printf("found on %i\n",i);
			/* Print packet */
	//		printf("\tData:");
	//		for (i=0; i<*payloadSize; i++) printf("%02x:", LLDPreceivedPayload[i]);
	//		printf("\n");
			return;
		}
	}

	close(sockfd[0]);	//TODO
}
