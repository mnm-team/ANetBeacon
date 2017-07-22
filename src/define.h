/**
 * @file define.h
 * @author Dominik Bitzer
 * @date 2017
 * @brief Contains application-wide includes with information such as addresses and TLV types.
 * 
 */

#ifndef DEFINE_H
#define DEFINE_H

#define _(STRING) gettext(STRING) /** @name Macro for gettext localization support */

// Protocol options such send frequency
#define LAN_BEACON_SEND_FREQUENCY 5	/** @name Send frequency in seconds */

/**
 * @name LAN-Beacon Multicast addresses and EtherTypes
 * @{
 */
#define LAN_BEACON_DEST_MAC	0xff, 0xff, 0xff, 0xff, 0xff, 0xff
//#define LAN_BEACON_DEST_MAC	0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e
//#define LAN_BEACON_DEST_MAC	0x01, 0x80, 0xc2, 0xde, 0x4c, 0x73
#define LAN_BEACON_ETHER_TYPE	0x88B5
#define CHALLENGE_ETHTYPE 0x88B6
// #define LAN_BEACON_ETHER_TYPE	0x88CC
/** @} */

/**
 * @name Buffer sizes
 * @{
 */
#define PARSED_TLVS_MAX_NUMBER	25
#define PARSED_TLVS_MAX_LENGTH	510

#define LAN_BEACON_BUF_SIZ 2000
#define KEY_PATHLENGTH_MAX 800
/** @} */

/**
 * @name Standard paths
 * @{
 */
#define PRIVATE_KEY_STANDARD_PATH "private_key.pem"
#define PUBLIC_KEY_STANDARD_PATH "public_key.pem"
/** @} */

/**
 * @name Display options
 * @{
 */
#define DEFAULT_SCROLLSPEED 2
#define SHOW_FRAMES_X_TIMES 3
#define DESCRIPTOR_WIDTH 10
/** @} */

/**
 * @name Subtype numbers lanbeacon
 * @{
 */
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
/** @} */

/**
 * @name Descriptor strings lanbeacon
 * @{
 */
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
/** @} */

#endif
