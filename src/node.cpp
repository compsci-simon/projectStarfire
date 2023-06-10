#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "logging.h"
#include "frame.h"

const int PORT = 12345;
const int BUFFER_SIZE = 1600;

class Node
{
  int sockfd;
  int port;
  char* host;
  char recv_buffer[PAYLOAD_SIZE + 26];
  char send_buffer[PAYLOAD_SIZE + 26] {0};
  struct sockaddr_in serverAddress;
  const char* name;
  bool listen;
  std::string macAddress;

  int sendMessageToServer(std::string message);
  void receive_messages_from_server();
  void echo_messages_received();
public:
  Node(int port, char *host, const char* name) {
    this->port = port;
    this->host = host;
    this->name = name;
  };
  int connect_to_server();
  void main_loop();
};

int Node::sendMessageToServer(std::string message) {
  return 0;
}

void Node::receive_messages_from_server() {
  memset(recv_buffer, 0, BUFFER_SIZE);
  if (read(sockfd, recv_buffer, BUFFER_SIZE) <= 0) {
    listen = false;
  }
}

void Node::echo_messages_received() {
  while (listen) {
    receive_messages_from_server();
    std::cout << recv_buffer << std::endl;
  }
}

int Node::connect_to_server() {

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      std::cerr << "Error creating socket" << std::endl;
      return 1;
    }

    // Bind socket to port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &(serverAddress.sin_addr)) < 0) {
      std::cerr << "Invalid address" << std::endl;
      return 1;
    }

    // Connect to server
    socklen_t serverAddressLen = sizeof(serverAddress);
    if (connect(sockfd, (struct sockaddr *) &serverAddress, serverAddressLen) < 0) {
      std::cerr << "Error connecting to the server" << std::endl;
      return 1;
    }
    Frame frame;
    const char* payload = "asdf";
    frame.set_payload((unsigned char*) payload);
    frame.to_string(send_buffer);
    send(sockfd, send_buffer, BUFFER_SIZE, 0);
    close(sockfd);
    return 0;
  }

void Node::main_loop() {
    std::string msg;
    std::thread thread(&Node::echo_messages_received, this);
    listen = true;
    while (listen) {
      std::getline(std::cin, msg);
      sendMessageToServer(msg);
      if (msg == "quit") {
        listen = false;
        break;
      }
    }
    thread.join();
  }

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "You must supply a name for this node" << std::endl;
    return 1;
  }
  Node node(PORT, (char *)"localhost", argv[1]);
  node.connect_to_server();
  // node.main_loop();
  return 0;
}