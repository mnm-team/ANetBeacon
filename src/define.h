#ifndef DEFINE_H
#define DEFINE_H

// Macro for gettext localization support
#define _(STRING) gettext(STRING)

#define PARSED_TLVS_MAX_NUMBER	25
#define PARSED_TLVS_MAX_LENGTH	510

// Display options
#define DESCRIPTOR_WIDTH 10
#define DEFAULT_SCROLLSPEED 5

#define LLDP_BUF_SIZ 2000
#define KEY_PATHLENGTH_MAX 500

#define PRIVATE_KEY_STANDARD_PATH "privkey.pem"
#define PUBLIC_KEY_STANDARD_PATH "pubkey.pem"

#define LLDP_SEND_FREQUENCY 1

// Subtype numbers lanbeacon:
#define SUBTYPE_VLAN_ID 200
#define SUBTYPE_NAME 201
#define SUBTYPE_CUSTOM 202
#define SUBTYPE_IPV4 203
#define SUBTYPE_IPV6 204
#define SUBTYPE_EMAIL 205
#define SUBTYPE_DHCP 206
#define SUBTYPE_ROUTER 207
#define SUBTYPE_SIGNATURE 216
#define SUBTYPE_COMBINED_STRING 217

// Descriptor strings lanbeacon:
#define DESCRIPTOR_VLAN_ID gettext("VLAN-ID:")
#define DESCRIPTOR_NAME gettext("VLAN-Name:")
#define DESCRIPTOR_CUSTOM gettext("Freetext:")
#define DESCRIPTOR_IPV4 gettext("IPv4:")
#define DESCRIPTOR_IPV6 gettext("IPv6:")
#define DESCRIPTOR_EMAIL gettext("Email:")
#define DESCRIPTOR_DHCP gettext("DHCP:")
#define DESCRIPTOR_ROUTER gettext("Router:")
#define DESCRIPTOR_SIGNATURE gettext("Authentication:")
#define DESCRIPTOR_COMBINED_STRING gettext("Combined String:")

#endif
