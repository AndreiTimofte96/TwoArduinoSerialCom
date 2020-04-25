#include "Error.hpp"

Error::Error() {
}

void Error::setError(errorMessages _error) {
  error = _error;
}

char const* Error::getError() {
  switch (error) {
    case CONNECT_PROTOCOL_NOT_CONNECTED_ERROR:
      return "CONNECT_PROTOCOL_NOT_CONNECTED_ERROR";

    case CONNECT_PROTOCOL_PROTOCOL_HAS_FINISHED:
      return "CONNECT_PROTOCOL_PROTOCOL_HAS_FINISHED";

    case WRITE_PROTOCOL_NOT_CONNECTED_ERROR:
      return "WRITE_PROTOCOL_NOT_CONNECTED_ERROR";

    case READ_PROTOCOL_NOT_CONNECTED_ERROR:
      return "READ_PROTOCOL_NOT_CONNECTED_ERROR";

    case CLOSE_PROTOCOL_NOT_CONNECTED_ERROR:
      return "CLOSE_PROTOCOL_NOT_CONNECTED_ERROR";

    case CONNECT_INTERNAL_ERROR:
      return "CONNECT_INTERNAL_ERROR";

    case WRITE_INTERNAL_ERROR:
      return "WRITE_INTERNAL_ERROR";

    case READ_INTERNAL_ERROR:
      return "READ_INTERNAL_ERROR";

    default:
      return "UNKNOWN_ERROR";
  }
}
