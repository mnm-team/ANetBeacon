.TH LANbeacon 8 "Dominik Bitzer"
.SH NAME
LANbeacon \- send out and authenticate custom LLDP packets containing machine- and humanreadable information VLANs \"TODO
.SH SYNOPSIS
.B LANbeacon
[\fB\-i\fR \fIVLAN_ID\fR]
[\fB\-n\fR \fIVLAN_NAME\fR]
[\fB\-4\fR \fIIPv4_SUBNETWORK\fR]
[\fB\-6\fR \fIIPv6_SUBNETWORK\fR]
[\fB\-e\fR \fIEMAIL_CONTACTPERSON\fR]
[\fB\-d\fR \fIDHCP_TYPES\fR]
[\fB\-r\fR \fIROUTER_INFORMATION\fR]
[\fB\-c\fR \fICUSTOM_STRING\fR]
.\" TODO signatures .IR file ...
.\" TODO create keys

.B LANbeacon
[\fB\-r\fR]
.\" TODO signatures .IR file ...
.\" TODO interactive mode

.B LANbeacon
[\fB\-h\fR]

.SH DESCRIPTION		\" TODO more info
.PP
In sending mode, \fBLANbeacon\fR periodically sends out a LLDP-package over all network interfaces.
The package contains all of the information that it was given using the parameters stated above.
.PP
In receiving mode, \fBLANbeacon\fR periodically checks all network interfaces for LLDP-packages and takes all known TLV subtypes to then store them for further processing.
Generally it will print them out in the standard output, if it is run on a microcomputer that supports the C-Berry display it will show the information there.
In case new packages arrive, it will update it's information that is being put out on the screen.
.SH OPTIONS		\" TODO options beschreiben
.TP
.BR \-n ", " \-\-bits =\fIBITS\fR
Set the number of bits to modify.
Default is one bit.



[-i VLAN_ID]
[-n VLAN_NAME]
.\" TODO	IP-Grammatik welche zusätzlichen Inhalte sind möglich?
[-4 IPv4_SUBNETWORK]	\"		TODO example		(e.g. 192.168.178.133/24)
[-6 IPv6_SUBNETWORK]	\"		TODO example
[-e EMAIL_CONTACTPERSON]
[-d DHCP_TYPES]
[-r ROUTER_INFORMATION]
[-c CUSTOM_STRING]

[-r]
[-h]


.SH EXAMPLES
To write some text to the standard output, use the
.BR printf (1)
command:
.PP
.nf
.RS
printf "this is a %s format %s\n" \\
usual string
.RE
.fi
.PP
Nifty, huh?


.\".nf turns off paragraph filling mode: we don't want that for showing command lines.
.\".fi turns it back on.
.\".RS starts a relative margin indent: examples are more visually distinguishable if they're indented.
.\".RE ends the indent.

.SH COMPILING
.PP 
use make for normal computers
.PP 
use make bananaLANbeacon to enable C-Berry support
.SH ENVIRONMENT
Keys?!
.SH SEE ALSO
lldpd


