
class UdpPacket {
 public:
  UdpPacket();

  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  int checkSum1;
  int checkSum2;
  int fromUAID;
  int toUAID;
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 49,
    BLOCK_HEADER_SIZE = 33,
    BLOCK_BODY_SIZE = 16
  };
};