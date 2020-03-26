#include <Arduino.h>

class Error {
 public:
  Error();

  enum errorMessages {
    CONNECTION_ERROR,
    READ_ERROR,
    WRITE_ERROR,

    TCP_CONNECTION_ERROR,
  };

  void setError(errorMessages _error);
  char const* getError();

 private:
  errorMessages error;
};
