CC = gcc
CFLAGS = -Wall -Werror -g
# CFLAGS = -std=c99 -Wall -Werror -g
OBJECTS = main.o# beacon.o

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
		rm -rf *.o

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf *.o LANbeacon
