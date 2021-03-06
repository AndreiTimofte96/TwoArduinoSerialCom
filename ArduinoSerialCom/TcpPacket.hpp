
class TcpPacket {
 public:
  TcpPacket();

  int pSize;      // 2
  int bLength;    //2,  length of a block: eg: 16 bytes or if it last block != 16 bytes
  int bNumber;    //3, number of blocks of the packet
  int bOffset;    //3 , offset of block x / bNumber
  int checkSum1;  //3,
  int checkSum2;  //3
  int UAID;       // 4
  char *bData;

  enum blockInformation {
    BLOCK_SIZE = 44,
    BLOCK_HEADER_SIZE = 27,
    BLOCK_BODY_SIZE = 17
  };
};