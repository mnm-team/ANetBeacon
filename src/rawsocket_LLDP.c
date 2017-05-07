#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
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
#define _GNU_SOURCE

#define LLDP_DEST_MAC	0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e
#define CHALLENGE_ETHTYPE 0x88B5
#define LLDP_ETHER_TYPE	0x88CC

#define LLDP_SEND_FREQUENCY 2
#define CHALLENGE_SEND_FREQUENCY 170000
#define CHALLENGE_SEND_TIMES 7

#define SEND_SOCKET 0
#define REC_SOCKET 1
#define SET_SELECT_FDS \
	FD_ZERO(&readfds); \
	for (int x = 0; x < numInterfaces; x++) \
		{FD_SET(sockfd[x], &readfds);}

void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct open_ssl_keys *lanbeacon_keys) {
	
	if (etherType == CHALLENGE_ETHTYPE)
		* (unsigned long *) payload = htonl(*(unsigned long *) payload);

	int sockfd[20];
	struct ifreq if_idx[20];
	struct ifreq if_mac[20];
	int frameLength = 0;
	int numInterfaces = 0;
	char lldpEthernetFrame[LLDP_BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) lldpEthernetFrame;
	struct sockaddr_ll socket_address;

	unsigned long *receivedChallenge = NULL;

	if (etherType == LLDP_ETHER_TYPE) {
		receivedChallenge = calloc(sizeof(unsigned long), 1);
		if(!receivedChallenge) 
			puts(_("calloc error of \"receivedChallenge\" in sendLLDPrawSock"));
	}

	// Construct the Ethernet header
	memset(lldpEthernetFrame, 0, LLDP_BUF_SIZ);
	memcpy(eh->ether_dhost, destination_mac, 6);

	// Ethertype field
	eh->ether_type = htons(etherType);
	frameLength += sizeof(struct ether_header);

	// Packet data
	memcpy(&lldpEthernetFrame[frameLength], payload, payloadLen);
	frameLength += payloadLen;

	// End of LLDPDU TLV
	if (etherType == LLDP_ETHER_TYPE) {
		lldpEthernetFrame[frameLength++] = 0x00;
		lldpEthernetFrame[frameLength++] = 0x00;
	}

	// Address length and destination MAC
	socket_address.sll_halen = ETH_ALEN;
	memcpy(socket_address.sll_addr, destination_mac, 6);

	// Get interfaces
	getInterfaces (sockfd, &numInterfaces, etherType, SEND_SOCKET, if_idx, if_mac, NULL, NULL);
	
	// Get interfaces

	// send challenge CHALLENGE_SEND_TIMES times
	// in case of LLDP-packet counter is not incremented, therefore infinite loop
	for (int i = 0; i<CHALLENGE_SEND_TIMES; ) {

		// send packets on all interfaces
		for(int j = 0; j < numInterfaces; j++) {

			// Ethernet header, destination MAC address
			memcpy(eh->ether_shost, ((uint8_t *)&if_mac[j].ifr_hwaddr.sa_data), 6);
			
			// Port and chassis subtype TLVs filled
			memcpy(&lldpEthernetFrame[17], ((uint8_t *)&if_mac[j].ifr_hwaddr.sa_data), 6);
			memcpy(&lldpEthernetFrame[26], ((uint8_t *)&if_mac[j].ifr_hwaddr.sa_data), 6);
			
			// Index of the network device
			socket_address.sll_ifindex = if_idx[j].ifr_ifindex;

			// Send packet
			if (sendto(sockfd[j], lldpEthernetFrame, frameLength, 0, 
				(struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
					printf(_("Send failed on interface number %i\n"), j);
			else
				printf(_("Successfully sent on interface number %i\n"), j);
		}

		if (etherType == CHALLENGE_ETHTYPE) {
			i++;
			usleep(CHALLENGE_SEND_FREQUENCY);
		}

		if (etherType == LLDP_ETHER_TYPE) {
			if (*receivedChallenge == 0) {
				*receivedChallenge = receiveChallenge();
				*receivedChallenge = htonl(*receivedChallenge);
				memcpy(&lldpEthernetFrame[frameLength-272+6], receivedChallenge, 4);
			}
			else
				sleep(LLDP_SEND_FREQUENCY);

			//## add signature ##//
			time_t timeStamp = time(NULL);
			timeStamp = htonl(timeStamp);
			memcpy(&lldpEthernetFrame[frameLength-272+6+4], &timeStamp, 4);
			unsigned char* sig = NULL;
			size_t slen = 0;
			signlanbeacon(&sig, &slen, (const unsigned char *) &lldpEthernetFrame[14], 
				(size_t) payloadLen - 256, lanbeacon_keys);
			memcpy(&lldpEthernetFrame[frameLength-272+6+4+4], sig, slen);
			free(sig);
		}

	}

	//free(receivedChallenge);

	return;
}


// parts of code based on https://gist.github.com/austinmarton/1922600
int sendLLDPrawSock (int LLDPDU_len, char *lanbeaconCustomTLVs, 
					struct open_ssl_keys *lanbeacon_keys)
{
	sendRawSocket ((unsigned char[6]){LLDP_DEST_MAC}, lanbeaconCustomTLVs, 
		LLDPDU_len, LLDP_ETHER_TYPE, lanbeacon_keys);
	return EXIT_SUCCESS;
}


// parts of code based on https://gist.github.com/austinmarton/2862515
struct received_lldp_packet *recLLDPrawSock(struct open_ssl_keys *lanbeacon_keys) {

	struct received_lldp_packet *my_received_lldp_packet = 
		malloc(sizeof(struct received_lldp_packet));
	if(!my_received_lldp_packet) 
		puts(_("malloc error of \"my_received_lldp_packet\" in recLLDPrawSock"));
	struct ether_header *eh = 
		(struct ether_header *) my_received_lldp_packet->lldpReceivedPayload;

	int sockfd[20];
	int sockopt[20];

	// parameters for select()
	int maxSockFd = 0;
	struct timeval tv = {1, 0};
	fd_set readfds;
	int numInterfaces = 0;

	getInterfaces (sockfd, &numInterfaces, LLDP_ETHER_TYPE, REC_SOCKET, NULL, NULL, sockopt, &maxSockFd);

	int challengeSentBool = 0;
	int rv;

	// receive one lanbeacon frame, send a challenge to the source MAC and
	// then wait for the frame containing the challenge
	for (int i = 0; i < 2; i++) {
		SET_SELECT_FDS
		rv = select(maxSockFd, &readfds, NULL, NULL, NULL);

		if (rv == -1) 
			perror("select");
		else if (rv == 0) 
			printf("Timeout occurred! No data after %i seconds.\n", LLDP_SEND_FREQUENCY);  // TODO receive for how long?
		else {
			for (int i = 0; i < numInterfaces; i++) {
				if (FD_ISSET(sockfd[i], &readfds)) {

					my_received_lldp_packet->payloadSize = 
						recvfrom(sockfd[i], my_received_lldp_packet->lldpReceivedPayload, 
							LLDP_BUF_SIZ, 0, NULL, NULL);

					// Check if the packet was sent to the LLDP multicast MAC address
					if(memcmp((unsigned char[6]){LLDP_DEST_MAC}, eh->ether_dhost, 6))
						continue;
					
					// Verify signature
					// position: end of LLDPDU - 2 Ethernet header - 14
					if (0 != verifylanbeacon(
						&my_received_lldp_packet->lldpReceivedPayload[14],
						my_received_lldp_packet->payloadSize - 2 - 14, lanbeacon_keys)) {
							puts("problem with verification");
					}

					break;
				}
			}
		}

		if (0 == challengeSentBool++) {
			// delete received packet, send challenge and flush buffer
			memset (my_received_lldp_packet->lldpReceivedPayload, 0, LLDP_BUF_SIZ);

			srand(time(NULL));
			// my_received_lldp_packet->challenge = 65321;
			my_received_lldp_packet->challenge = 1+ (rand() % 4294967294);
			sendRawSocket (eh->ether_shost, &my_received_lldp_packet->challenge, 
				4, CHALLENGE_ETHTYPE, NULL);


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
					for (int j = 0; j < numInterfaces; j++) {
						if (FD_ISSET(sockfd[j], &readfds)) {
		puts("Deleted something");
							recvfrom(sockfd[j], NULL, 1500, 0, NULL, NULL);
						}
					}
				}
			}
		}
		else break;
	}

	for (int i = 0; i < numInterfaces; i++)
		close(sockfd[i]);

	return my_received_lldp_packet;
}


// parts of code based on https://gist.github.com/austinmarton/2862515
unsigned long receiveChallenge() {

	unsigned char *receiveBuf = calloc(300, 1);
	if(!receiveBuf) puts(_("calloc error of \"receiveBuf\" in receiveChallenge"));
	int receivedSize;
	unsigned long *receivedChallenge = calloc(sizeof(unsigned long), 1);
	if(!receivedChallenge) puts(_("calloc error of \"receivedChallenge\" in receiveChallenge"));

	struct ether_header *eh = (struct ether_header *) receiveBuf;
	int sockfd[20];
	int sockopt[20];

	// parameters for select()
	int maxSockFd = 0;
	struct timeval tv = {1, 0};
	tv.tv_sec = LLDP_SEND_FREQUENCY;
	fd_set readfds;

	int numInterfaces = 0;

	getInterfaces (sockfd, &numInterfaces, CHALLENGE_ETHTYPE, REC_SOCKET, 
		NULL, NULL, sockopt, &maxSockFd);

	SET_SELECT_FDS

	int rv = select(maxSockFd, &readfds, NULL, NULL, &tv);

	if (rv == -1) 
		perror("select"); // error occurred in select()
	else if (rv == 0) 
		printf("Timeout occurred! No data after %i seconds.\n", LLDP_SEND_FREQUENCY);
	else {
		for (int i = 0; i < numInterfaces; i++) {
			if (FD_ISSET(sockfd[i], &readfds)) {

				receivedSize = recvfrom(sockfd[i], receiveBuf, 300, 0, NULL, NULL);

				memcpy(receivedChallenge, &receiveBuf[14], 4);
				*receivedChallenge = ntohl(*receivedChallenge);
				printf(_("Received challenge: %lu\n"), *receivedChallenge);

				break;
			}
		}
	}

	for (int i = 0; i < numInterfaces; i++)
		close(sockfd[i]);

	free(receiveBuf);

	return *receivedChallenge;
}


void getInterfaces (int *sockfd, int *numInterfaces, unsigned short etherType, 
					unsigned short sendOrReceive, struct ifreq *if_idx, 
					struct ifreq *if_mac, int *sockopt, int *maxSockFd) {

	struct ifaddrs *interfaces;
	if (getifaddrs(&interfaces) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	for (; interfaces != NULL; interfaces = interfaces->ifa_next) {

		if (interfaces->ifa_addr == NULL) continue;
		if (!(interfaces->ifa_addr->sa_family == AF_PACKET)) continue;

		// Open RAW socket to send on
		if ((sockfd[*numInterfaces] = socket(PF_PACKET, SOCK_RAW, htons(etherType))) == -1) {
			perror("socket");
		}

		if (sendOrReceive == SEND_SOCKET) {

			// Get the index of the interface to send on
			memset(&if_idx[*numInterfaces], 0, sizeof(struct ifreq));
			memcpy(if_idx[*numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(sockfd[*numInterfaces], SIOCGIFINDEX, &if_idx[*numInterfaces]) < 0)
				perror("SIOCGIFINDEX");
			// Get the MAC address of the interface to send on
			memset(&if_mac[*numInterfaces], 0, sizeof(struct ifreq));
			memcpy(if_mac[*numInterfaces].ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			if (ioctl(sockfd[*numInterfaces], SIOCGIFHWADDR, &if_mac[*numInterfaces]) < 0)
				perror("SIOCGIFHWADDR");
		}

		if (sendOrReceive == REC_SOCKET) {

			// Set interface to promiscuous mode
			struct ifreq ifopts;
			memcpy(ifopts.ifr_name, interfaces->ifa_name, IFNAMSIZ-1);
			ioctl(sockfd[*numInterfaces], SIOCGIFFLAGS, &ifopts);
			ifopts.ifr_flags |= IFF_PROMISC;
			ioctl(sockfd[*numInterfaces], SIOCSIFFLAGS, &ifopts);

			// Allow the socket to be reused - incase connection is closed prematurely
			if (setsockopt(sockfd[*numInterfaces], SOL_SOCKET, SO_REUSEADDR, 
				&sockopt[*numInterfaces], sizeof sockopt) == -1) {
					perror("setsockopt");
					close(sockfd[*numInterfaces]);
					exit(EXIT_FAILURE);
			}

			// Bind to device
			if (setsockopt(sockfd[*numInterfaces], SOL_SOCKET, SO_BINDTODEVICE, 
				interfaces->ifa_name, IFNAMSIZ-1) == -1)	{
					perror("SO_BINDTODEVICE");
					close(sockfd[*numInterfaces]);
					exit(EXIT_FAILURE);
			}

			if (sockfd[*numInterfaces] > *maxSockFd)
				*maxSockFd = sockfd[*numInterfaces];
		}

		printf(_("Number %i is interface %s\n"), *numInterfaces, interfaces->ifa_name);
		*numInterfaces = *numInterfaces + 1;
	}

	return;
}

