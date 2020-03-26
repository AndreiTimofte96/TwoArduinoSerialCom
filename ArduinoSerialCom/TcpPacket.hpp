
class TcpPacket {
 public:
  TcpPacket();

  int pSize;
  int bLength;  //length of a block: eg: 16 bytes or if it last block != 16 bytes
  int bNumber;  //number of blocks of the packet
  int bOffset;  // offset of block x / bNumber
  int checkSum1;
  int checkSum2;
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 42,
    BLOCK_HEADER_SIZE = 25,
    BLOCK_BODY_SIZE = 17
  };
};