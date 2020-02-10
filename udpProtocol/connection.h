class Connection {
 public:
  Connection();
  enum connectionStatus {
    DISCONNECTED,
    CONNECTED,
    IDLE,
  };
  void setStatus(connectionStatus status);
  connectionStatus getStatus();

 private:
  connectionStatus status;
};