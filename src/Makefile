CC = gcc
CFLAGS = -Wall -Werror -g -Wno-unused-variable
# CFLAGS = -std=c99 -Wall -Werror -g
OBJECTS = main.o tools.o beacon.o

LANbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o LANbeacon
		rm -rf *.o

%.o: %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf *.o LANbeacon
