#ifndef CONFIG_H
#define CONFIG_H

#define INPUT_STRINGS_SIZE 100

struct LANbeaconProperties {
	
	char VLAN_id[INPUT_STRINGS_SIZE];
	char VLAN_name[INPUT_STRINGS_SIZE];
	char Custom_Text[INPUT_STRINGS_SIZE];
	char organization_identifier[INPUT_STRINGS_SIZE];
	
};


struct LANbeaconProperties *setLANbeaconProperties(int *argc, char **argv);
void printHelp();


#endif

