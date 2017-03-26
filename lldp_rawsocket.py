# Coded using: http://www.binarytides.com/python-packet-sniffer-code-linux/

import struct
import socket
import binascii

import fcntl
try:
    import ctypes
    class ifreq(ctypes.Structure):
        _fields_ = [("ifr_ifrn", ctypes.c_char * 16),
                    ("ifr_flags", ctypes.c_short)]
except (ImportError, NameError) as e:
    print ("Meh")
#Taken from the C Header Files 
ETH_P_ALL = 0x0003
IFF_PROMISC = 0x100
SIOCGIFFLAGS = 0x8913
SIOCSIFFLAGS = 0x8914

# Enable promiscuous mode from http://stackoverflow.com/a/6072625
def promiscuous_mode(interface, sock, enable=False):
    ifr = ifreq()
    ifr.ifr_ifrn = interface
    fcntl.ioctl(sock.fileno(), SIOCGIFFLAGS, ifr)
    if enable:
        ifr.ifr_flags |= IFF_PROMISC
    else:
        ifr.ifr_flags &= ~IFF_PROMISC
    fcntl.ioctl(sock.fileno(), SIOCSIFFLAGS, ifr)

def getLLDP ()
  #create an INET, raw socket
  lldp_rawsocket = socket.socket( socket.AF_PACKET , socket.SOCK_RAW , socket.ntohs(0x0003))
  lldp_rawsocket.bind(('enp0s18', 0x0003))
  promiscuous_mode('enp0s18', lldp_rawsocket, True)

  # receive a packet
  while True:
    my_raw_packet = lldp_rawsocket.recvfrom(65565)
    
    test = my_raw_packet[0]
  #  print (str(my_raw_packet[1]) + str(binascii.hexlify(test)))
     
    ethernetHeaderTotal = test[0:14]
    lldpPayload = test[14:]
    ethernetHeaderUnpacked = struct.unpack("!6s6s2s", ethernetHeaderTotal)
    ethernetHeaderProtocol = ethernetHeaderUnpacked[2]
    
  #  
  #  print(lldpPayload)
  #  
    if ethernetHeaderProtocol == '\x88\xCC':
      print(binascii.hexlify(ethernetHeaderProtocol))
      print(ethernetHeaderProtocol)
      print(binascii.hexlify(lldpPayload))

