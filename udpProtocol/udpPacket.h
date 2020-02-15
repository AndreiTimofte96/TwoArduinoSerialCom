
class UdpPacket {
 public:
  UdpPacket();

  int pSize;
  int bLength;
  int bNumber;
  int bOffset;
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 32,
    BLOCK_HEADER_SIZE = 16,
    BLOCK_BODY_SIZE = 16
  };
};