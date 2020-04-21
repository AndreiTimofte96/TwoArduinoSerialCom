class Connection {
 public:
  Connection();
  enum connectionStatus {
    DISCONNECTED,
    CONNECTED,
    FINISHED,
    ERROR
  };

  // DISCONNECTED -> CONNECTED -> FINISHED  || ERROR

  void setStatus(connectionStatus status);
  connectionStatus getStatus();

 private:
  connectionStatus status;
};