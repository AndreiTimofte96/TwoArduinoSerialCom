
class TcpConnectionPacket {
 public:
  TcpConnectionPacket();

  int syn;
  int seq;  // /ACK:0, SYN:1 CON:2 FIN:3 ERR: 9
  int ack;

  enum blockInformation {
    BLOCK_SIZE = 12,
  };
};