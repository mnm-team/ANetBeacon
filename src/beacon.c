
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"


struct LANbeacon createLANbeacon()
{
	struct LANbeacon myLANbeacon;
	
	myLANbeacon.TLVtype = 127;
	strcpy(myLANbeacon.TLVorganizationIdentifier,"LMU");
	
	return myLANbeacon;
	
}

