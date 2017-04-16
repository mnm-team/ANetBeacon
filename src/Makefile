CC = gcc
OBJECTS = main.o sender.o rawsocket_LLDP.o receiver.o openssl_sign.o
BANANAOBJECTS = cberry_includes_tft.o cberry_includes_RAIO8870.o
LANbeacon:			CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable -lcrypto
bananaLANbeacon:	CFLAGS = -Wall -Werror -g -std=c11 -Wno-unused-variable -Wno-unused-but-set-variable -lcrypto -DBANANAPI_SWITCH

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

tft.o: cberry_includes_tft.c cberry_includes_tft.h
	gcc -Os -c cberry_includes_tft.c

RAIO8870.o: cberry_includes_RAIO8870.c cberry_includes_RAIO8870.h
	gcc -Os -c cberry_includes_RAIO8870.c

openssl_sign.o: openssl_sign.c openssl_sign.h
	gcc -lcrypto -std=c11 -c openssl_sign.c

translations: 
		xgettext --keyword=_ --language=C --add-comments --sort-output -o l10n/LANbeacon.pot sender.c
		msgmerge --output-file=l10n/de/LANbeacon.po l10n/de/LANbeacon.po l10n/LANbeacon.pot
		msgfmt --output-file=l10n/de/LC_MESSAGES/LANbeacon.mo l10n/de/LANbeacon.po

clean:
		rm -rf *.o LANbeacon
