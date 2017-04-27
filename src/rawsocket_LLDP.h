#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs);
struct received_lldp_packet *my_received_lldp_packet recLLDPrawSock(struct open_ssl_keys *lanbeacon_keys);

#endif
