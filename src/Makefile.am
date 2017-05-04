CC = gcc
OBJECTS = main.o sender.o rawsocket_LLDP.o receiver.o openssl_sign.o
BANANAOBJECTS = cberry_includes_tft.o cberry_includes_RAIO8870.o

bin_PROGRAMS = lanbeacon
lanbeacon_SOURCES = main.c sender.c sender.h rawsocket_LLDP.c rawsocket_LLDP.h receiver.c receiver.h openssl_sign.c openssl_sign.h cberry_includes_tft.c cberry_includes_tft.h cberry_includes_RAIO8870.c cberry_includes_RAIO8870.h

lanbeacon:			CFLAGS = -Wall -Werror -g -std=gnu11 -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-value -lcrypto
bananalanbeacon:	CFLAGS = -Wall -Werror -g -std=gnu11 -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-value -lcrypto -DBANANAPI_SWITCH

lanbeacon: $(OBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) -o lanbeacon
		rm -rf *.o
		rm -rf a.out

bananalanbeacon: $(OBJECTS) $(BANANAOBJECTS)
		$(CC) $(CFLAGS) $(OBJECTS) $(BANANAOBJECTS) -lwiringPi -lrt -lm -o lanbeacon
		rm -rf *.o
		rm -rf a.out

%.o: %.c
		$(CC) $(CFLAGS) -c $<

tft.o: cberry_includes_tft.c cberry_includes_tft.h
	gcc -Os -c cberry_includes_tft.c

RAIO8870.o: cberry_includes_RAIO8870.c cberry_includes_RAIO8870.h
	gcc -Os -c cberry_includes_RAIO8870.c

openssl_sign.o: openssl_sign.c openssl_sign.h
	gcc -lcrypto -std=gnu11 -c openssl_sign.c

translations: 
		xgettext --keyword=_ --language=C --add-comments --sort-output -o l10n/lanbeacon.pot define.h receiver.c sender.c main.c
		msgmerge --output-file=l10n/de/lanbeacon.po l10n/de/lanbeacon.po l10n/lanbeacon.pot
		msgfmt --output-file=l10n/de/LC_MESSAGES/lanbeacon.mo l10n/de/lanbeacon.po

doxygen:
		cd ../doc/ && \
		doxygen doxygenConfig_lanbeacon && \
		cd latex/ && \
		make && \
		cp refman.pdf ../doxygen_lanbeacon.pdf

clean:
		rm -rf *.o lanbeacon
