#include "ArduinoSerialCom.hpp"
#include "UdpPacket.hpp"

class UdpProtocol : public ArduinoSerialCom {
  UdpPacket packetRead;
  UdpPacket packetWrite;

  void formatSendData(char *packet, int length);
  int sendData(char *dataToSend, int fromUAID, int toUAID);

  void formatReceiveData(char *bData, char *dataToReceive);
  int receiveData(char *dataToReceive, int &fromUAID, int &toUAID);

 public:
  UdpProtocol();
  int connect();
  int write(char *dataToSend, int toUAID);
  int write(char *dataToSend, int fromUAID, int toUAID);
  int write(int dataToSend, int toUAID);
  int write(int dataToSend, int fromUAID, int toUAID);
  int write(float dataToSend, int toUAID);
  int write(float dataToSend, int fromUAID, int toUAID);

  int read(char *dataToReceive, int &fromUAID);
  int read(char *dataToReceive, int &fromUAID, int &toUAID);
  int read(int &dataToReceive, int &UAID);
  int read(int &dataToReceive, int &fromUAID, int &UAID);

  int read(float &dataToReceive, int &UAID);
  int read(float &dataToReceive, int &fromUAID, int &UAID);

  int clientClose();
  int serverClose();

  // template <typename T>
  // int write(T dataToSend, int UAID) {
  //   int status = checkConnectionStatus(Error::WRITE_PROTOCOL_NOT_CONNECTED_ERROR);
  //   if (status < 0) return status;

  //   softwareSerial->listen();
  //   printlnLog(F("\nSENDING:"));

  //   unsigned char *ptr = (unsigned char *)&dataToSend;
  //   int dataToSendLength = sizeof(dataToSend);
  //   char dataToSendString[dataToSendLength + 1];

  //   for (int index = 0; index < dataToSendLength; index++) {
  //     dataToSendString[index] = *ptr++;
  //   }
  //   return sendData(dataToSendString, UAID);
  // }

  // template <typename T>
  // int read(T &dataToReceive, int &UAID) {
  //   int status = checkConnectionStatus(Error::READ_PROTOCOL_NOT_CONNECTED_ERROR);
  //   if (status < 0) return status;

  //   softwareSerial->listen();
  //   printlnLog(F("\nRECEIVING:"));

  //   char *dataToReceiveString;
  //   dataToReceiveString = (char *)malloc(sizeof(char) * 200);
  //   memset(dataToReceiveString, '\0', sizeof(char) * 200);
  //   int length = receiveData(dataToReceiveString, UAID);

  //   unsigned char *ptr = (unsigned char *)&dataToReceive;
  //   for (int index = 0; index < length; index++) {
  //     *ptr++ = dataToReceiveString[index];
  //   }
  //   free(dataToReceiveString);
  //   return length;
  // }
};
