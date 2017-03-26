import binascii, re, os, math, textwrap, time
# import lldp_rawsocket.py
BREITE = 10

def main():
  ### Get content of LLDP packet in Hex format and then decode it
  
#  lldp_rawsocket.getLLDP
  
  hexLLDPcontent = getLLDPcontent ('020704bc5ff414346d040703bc5ff414346d060200140a06636f6e7261640c654665646f72612032332028576f726b73746174696f6e2045646974696f6e29204c696e757820342e372e332d3130302e666332332e7838365f363420233120534d50205765642053657020372031373a32363a3431205554432032303136207838365f36340e04009c0014100c0501c0a8b27502000000020010181102fe80000000000000be5ff4fffe14346d0200000002000806656e70307337fe0900120f030100000000fe0900120f01036c000010ff05cc4d55d9007b084d4e4d2d564c414e2e20495076343a203139322e3136382e3137382e3133332f32342e20495076363a20323030313a3631383a31313a313a313a3a312f3132372e20456d61696c3a20646f6d696e696b2e6269747a6572406d61696c626f782e6f72672e20444843503a204448435020696e666f2e20526f757465723a204d41433a2030303a30343a34623a30313a37303a61612e20437573746f6d3a20446173206973742065696e20426569737069656c20667565722065696e656e2062656e75747a6572646566696e69657274656e20537472696e672e204f55492b737562747970653a204c4d55203231372e20564c414e2049443a203132332e200000')
  # normal ohne Python RAW-Sockets:('02070400044b0170ac04070300044b0170aa060200780a096c6f63616c686f73740c674665646f72612032352028576f726b73746174696f6e2045646974696f6e29204c696e757820342e392e31342d3230302e666332352e7838365f363420233120534d50204d6f6e204d61722031332031393a32363a3430205554432032303137207838365f36340e04009c0014100c0501c0a8b28502000000030010181102fe800000000000000f94918e4bd31ccc0200000003000807656e7030733138fe0900120f030100000000fe0900120f01036c010010ff13cc4d55d9007b084d4e4d2d564c414e2e20495076343a203139322e3136382e3137382e3133332f32342e20495076363a20323030313a3631383a31313a313a313a3a312f3132372e20456d61696c3a20646f6d696e696b2e6269747a6572406d61696c626f782e6f72672e20444843503a204448435020696e666f2e20526f757465723a204d41433a2030303a30343a34623a30313a37303a61612e20437573746f6d3a20446173206973742065696e20426569737069656c20667565722065696e656e2062656e75747a6572646566696e69657274656e20537472696e672e204f7267616e697a6174696f6e616c206964656e7469666965723a204c4d55203231372e20564c414e2049443a203132332e200000')
	  
  stringLLDPcontent = binascii.unhexlify(hexLLDPcontent[6:]).decode('ascii')

  ### Git information that is not stored as cleartext in LLDP packet
  VLANid = int.from_bytes(binascii.unhexlify(hexLLDPcontent[:4]), byteorder='big', signed=False)
  VLANnameLength = int.from_bytes(binascii.unhexlify(hexLLDPcontent[4:6]), byteorder='big', signed=False)
  VLANname = stringLLDPcontent[:VLANnameLength]

  #debugprint	print("1111111111222222222233333333334444444444")

  ### Split up LLDP string to single parts of information
  inhalteZumAusgeben = []
  inhalteZumAusgeben.extend(stringLLDPcontent.split(". "))    # Rest of split up raw string
  inhalteZumAusgeben[0] = "VLANname: " + inhalteZumAusgeben[0]
  inhalteZumAusgeben.insert(1,"VLANid: " + str(VLANid))

  #debugprint print("hexLLDPcontent: " + hexLLDPcontent)
  #debugprint print("stringLLDPcontent: " + stringLLDPcontent)
  #debugprint print(inhalteZumAusgeben)

  ###  Split up text lines so that they fit on C-Berry screen
  laengeListe = len(inhalteZumAusgeben)
  for element_nr in range(len(inhalteZumAusgeben)):
    ort = inhalteZumAusgeben[element_nr].find(": ")
    if ort>-1: 
      inhalteZumAusgeben[element_nr] = inhalteZumAusgeben[element_nr][:ort+2].ljust(11) + inhalteZumAusgeben[element_nr][ort+2:]
    inhalteZumAusgeben.extend(textwrap.TextWrapper(subsequent_indent=" ",width=40).fill(inhalteZumAusgeben[element_nr]).split("\n"))
  del inhalteZumAusgeben[:laengeListe]

  #debugprint  print(inhalteZumAusgeben)

  ### Print as many lines at a time as they fit on the C-Berry screen
  while True:
    os.system('clear')
    for element_nr in range(len(inhalteZumAusgeben)):
      if len(inhalteZumAusgeben[element_nr]) == 0: 
        continue
      if element_nr!=0 and (element_nr%5 == 0):
        time.sleep(2)
        os.system('clear')
      print(inhalteZumAusgeben[element_nr])
    time.sleep(2)

### This function looks for LLDP-TLVs with OUI cc4d55d9 and LLDP-type 127, then extracts contents (without Custom-TLV header)
def getLLDPcontent(hexLLDPpaket):
  match = re.search(r"(ff|fe)..cc4d55d9", hexLLDPpaket)
  hexLLDPpayload = hexLLDPpaket[match.start(1):]
#debugprint  print("hexLLDPpayload: " + hexLLDPpayload)
  
  TLVsizeAndType = binascii.unhexlify(hexLLDPpayload[:4])
  TLVsizeAndType = int.from_bytes(TLVsizeAndType, byteorder='big', signed=False)
  TLVsize = TLVsizeAndType & 0b111111111
  
  return hexLLDPpayload[4+8:4+TLVsize*2]  # 4 Size + Type, 8 OUI + Subtype

main()

'''
with AlterCode:
  from scapy.all import *
  INFILE = 'meindump_neu'
  OUTFILE = 'stripped.pcap'
  paks = rdpcap(INFILE)
  myLLDPpacket = paks[0].payload
  
  #for pak in paks:
  #    pak[TCP].remove_payload()
  #wrpcap(OUTFILE, paks)

  

  filename = 'meindump_neu'
  with open(filename, 'rb') as f:
    content = f.read()
  hexLLDPpaket = binascii.hexlify(content)


  def getNextPDUstring(stringLLDPcontent)
    match = re.search(r"(fe|ff)..cc4d55d9", hexLLDPpaket)
    result =
    
    return result
'''

