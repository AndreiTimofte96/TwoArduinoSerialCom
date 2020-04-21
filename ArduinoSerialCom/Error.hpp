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

    CONNECT_INTERNAL_ERROR,
    WRITE_INTERNAL_ERROR,
    READ_INTERNAL_ERROR,
  };

  void setError(errorMessages _error);
  char const* getError();

 private:
  errorMessages error;
};
