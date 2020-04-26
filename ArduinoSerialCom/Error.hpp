#include <Arduino.h>

class Error {
 public:
  Error();

  enum errorMessages {
    CONNECT_PROTOCOL_NOT_CONNECTED_ERROR,
    CONNECT_PROTOCOL_PROTOCOL_HAS_FINISHED,

    WRITE_PROTOCOL_NOT_CONNECTED_ERROR,
    READ_PROTOCOL_NOT_CONNECTED_ERROR,
    CLOSE_PROTOCOL_NOT_CONNECTED_ERROR,

    LISTEN_INTERNAL_ERROR,
    CONNECT_INTERNAL_ERROR,
    WRITE_INTERNAL_ERROR,
    READ_INTERNAL_ERROR,
    CLIENT_CLOSE_INTERNAL_ERROR,
    SERVER_CLOSE_INTERNAL_ERROR,
  };

  enum errorCodes {
    ERROR = -3,
    DISCONNECTED = -2,
    FINISHED = -1,
    CONNECTED = 1,
    CLOSED_CONN = 1,
  };

  void setError(errorMessages _error);
  char const* getError();

 private:
  errorMessages error;
};
