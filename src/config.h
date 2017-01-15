#ifndef CONFIG_H
#define CONFIG_H

#define INPUT_STRINGS_SIZE 100

struct LANbeaconProperties {
	
	char VLAN_id[INPUT_STRINGS_SIZE];
	char VLAN_name[INPUT_STRINGS_SIZE];
	char Custom_Text[INPUT_STRINGS_SIZE];
	
	
	// TODO: alte rausschmeißen:
	char	*gamekindname;
	unsigned int	portnumber;
	char	*hostname;
	char	*gameid;
	char	*clientversion;
	int		debugmode;
};


struct LANbeaconProperties *setLANbeaconProperties(int *argc, char **argv);
void printHelp();


#endif

