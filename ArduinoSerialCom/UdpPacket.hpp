
class UdpPacket {
 public:
  UdpPacket();

  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  int checkSum1;
  int checkSum2;
  int UAID;
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 45,
    BLOCK_HEADER_SIZE = 29,
    BLOCK_BODY_SIZE = 16
  };
};