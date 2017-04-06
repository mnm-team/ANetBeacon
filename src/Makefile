CC = gcc
CFLAGS = -Wall -Werror -g -Wno-unused-variable -Wno-unused-but-set-variable
# CFLAGS = -std=c99 -Wall -Werror -g
OBJECTS = main.o tools.o beacon.o config.o mergedBeacon.o sendLLDPrawSock.o

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
		rm -rf *.o
		rm -rf a.out

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf *.o LANbeacon
