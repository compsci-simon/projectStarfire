#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <thread>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

class Router {
private:
  int sockfd;
  struct sockaddr_in serverAddress, clientAddress;

  static void handleConnection(int sockfd) {
    while (true) {
      // Receive and print messages
      int valRead;
      char buffer[BUFFER_SIZE];
      valRead = read(sockfd, buffer, BUFFER_SIZE);
      if (valRead < 0) {
        std::cerr << "Error reading value" << std::endl;
        continue;
      }
      std::cout << "Received from client " << sockfd << ": " << buffer << std::endl;
      send(sockfd, buffer, BUFFER_SIZE, 0);
    }

    close(sockfd);
  }
public:

  bool accept_connections() {
    int addrLen = sizeof(serverAddress);
    std::vector<int> clients;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      std::cerr << "Error creating socket" << std::endl;
      return false;
    }
    // Bind socket to IP and port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serverAddress, addrLen) < 0) {
      std::cerr << "Error binding socket" << std::endl;
      return false;
    }

    if (listen(sockfd, 3) < 0) {
      std::cerr << "Listen failed" << std::endl;
      return false;
    }

    socklen_t client_len = sizeof(clientAddress);
    while (true) {
      int temp = accept(sockfd, (struct sockaddr *)&clientAddress, &client_len);
      std::cout << "Accepted connection" << std::endl;
      if (temp < 0) {
        std::cerr << "Accept failed" << std::endl;
        return false;
      } else {
        clients.push_back(temp);
        std::thread thread(handleConnection, temp);
      }
    }
  }
};
int main()
{
  Router router;
  router.accept_connections();
  return 0;
}