CC = gcc
OBJECTS = main.o mergedBeacon.o LLDPrawSock.o evaluateLANbeacon.o bananaPIprint.o 
BANANAOBJECTS = tft.o RAIO8870.o
LANbeacon:			CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable
bananaLANbeacon:	CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable -DBANANAPI_SWITCH

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
		rm -rf *.o
		rm -rf a.out

bananaLANbeacon: $(OBJECTS) $(BANANAOBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) $(BANANAOBJECTS) -lwiringPi -lrt -lm -o LANbeacon
		rm -rf *.o
		rm -rf a.out

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf *.o LANbeacon
