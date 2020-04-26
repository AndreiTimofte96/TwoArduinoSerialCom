#include "Connection.hpp"
Connection::Connection() {
}

void Connection::setStatus(connectionStatus _status) {
  status = _status;
}

Connection::connectionStatus Connection::getStatus() {
  return status;
}

const char* Connection::getStatusStringified() {
  switch (status) {
    case ERROR:
      return "ERROR";
    case DISCONNECTED:
      return "DISCONNECTED";
    case FINISHED:
      return "FINISHED";
    case CONNECTED:
      return "CONNECTED";
  }
}