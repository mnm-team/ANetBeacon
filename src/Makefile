CC = gcc
OBJECTS = main.o mergedBeacon.o LLDPrawSock.o evaluateLANbeacon.o bananaPIprint.o openssl_sign.o
BANANAOBJECTS = tft_cberry_includes/tft.o tft_cberry_includes/RAIO8870.o
LANbeacon:			CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable -lcrypto
bananaLANbeacon:	CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable -lcrypto -DBANANAPI_SWITCH

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
#		rm -rf *.o
		rm -rf a.out

bananaLANbeacon: $(OBJECTS) $(BANANAOBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) $(BANANAOBJECTS) -lwiringPi -lrt -lm -o LANbeacon
#		rm -rf *.o
		rm -rf a.out

%.o: %.c
		$(CC) $(CFLAGS) -c $<

tft.o: tft.c tft.h
	gcc -Os -c tft_cberry_includes/tft.c

RAIO8870.o: RAIO8870.c RAIO8870.h
	gcc -Os -c tft_cberry_includes/RAIO8870.c

openssl_sign.o: openssl_sign.c openssl_sign.h
	gcc -lcrypto -std=c11 -c openssl_sign.c

clean:
		rm -rf *.o LANbeacon
