#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H

char ** evaluateLANbeacon (unsigned char *LLDPreceivedPayload, ssize_t payloadSize);
void bananaPIprint (char **parsedBeaconContents, struct open_ssl_keys *lanbeacon_keys);

#endif
