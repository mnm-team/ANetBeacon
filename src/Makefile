CC = gcc
CFLAGS = -Wall -Werror -g -std=c11 #-std=c99 #-Wno-unused-variable -Wno-unused-but-set-variable
# CFLAGS = -std=c99 -Wall -Werror -g
OBJECTS = main.o mergedBeacon.o LLDPrawSock.o evaluateLANbeacon.o #tools.o beacon.o config.o

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
		rm -rf *.o
		rm -rf a.out

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf *.o LANbeacon
