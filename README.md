# LAN-Beacon

Send out or receive optionally authenticated, custom-fillable LLDP-like frames containing machine- and humanreadable information about VLANs and network topology.

In sender mode, LAN-Beacon periodically sends out a Ethernet-frames over all network interfaces or one selected interface. The frame contains all of the information that it was given using the parameters stated above. The same type of content can be sent multiple times

In receiver mode, LAN-Beacon periodically checks all network interfaces for LAN-Beacon frames and evaluates all known TLV subtypes. Generally it will print out the received information on the standard output, if it is run on a microcomputer that supports the C-Berry display it will show the information there.

In case new frames arrive, it will update it's information that is being put out on the screen.

Receiving from multiple VLANs at once is supported.

__Usage:__ 

lanbeacon [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_SUBNETWORK] [-6 IPv6_SUBNETWORK] [-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING] [-f SENDING_INTERFACE] [-g] -p PRIVATE_KEY_PASSWORD [-s PATH_TO_PRIVATE_KEY] [-v PATH_TO_PUBLIC_KEY] [-z SEND_FREQUENCY]

lanbeacon -l [-a] [-v PATH_TO_PUBLIC_KEY] [-y SCROLL_SPEED]

lanbeacon [-h]
