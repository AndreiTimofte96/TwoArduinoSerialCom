
class UdpPacket {
 public:
  UdpPacket();

  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  int checkSum1;
  int checkSum2;
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 41,
    BLOCK_HEADER_SIZE = 25,
    BLOCK_BODY_SIZE = 16
  };
};