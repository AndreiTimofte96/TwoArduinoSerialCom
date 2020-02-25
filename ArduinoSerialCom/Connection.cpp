#include "Connection.hpp"
Connection::Connection() {
}

void Connection::setStatus(connectionStatus _status) {
  status = _status;
}

Connection::connectionStatus Connection::getStatus() {
  return status;
}
