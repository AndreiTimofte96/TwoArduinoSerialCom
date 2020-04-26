class Connection {
 public:
  Connection();
  enum connectionStatus {
    ERROR,         // error code: -3
    DISCONNECTED,  //errod code: -2
    FINISHED,      // error code: -1
    CONNECTED,     // error code: 1
  };

  // DISCONNECTED -> CONNECTED -> FINISHED  || ERROR

  void setStatus(connectionStatus status);
  connectionStatus getStatus();
  const char* getStatusStringified();

 private:
  connectionStatus status;
};