/* Modification of a basic client application for usage in a chat app */

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "messaging.cpp"

const int port = 8080;

void conn_listen(int fd) {
  std::string recieved{chat::receive_msg(fd)};
  while (recieved != "") {
    std::cout << recieved << "\n";
    recieved = chat::receive_msg(fd);
  }
}

int main(int argc, char *argv[]) {
  int sockfd, numbytes;
  struct sockaddr_in server_address;
  char buffer[16] = {0};

  if (argc != 3) {
    std::cerr << "usage: client hostname username\n";
    return -1;
  }

  std::string hostname{argv[1]};
  if (hostname == "localhost")
    hostname = "127.0.0.1";

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Error creating socket.\n";
    return -1;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  if (inet_pton(AF_INET, hostname.c_str(), &server_address.sin_addr) <= 0) {
    std::cerr << "Invalid address\n";
    return -1;
  }

  if (connect(sockfd, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0) {
    std::cerr << "Connection failed\n";
    return -1;
  }

  std::thread thr{conn_listen, sockfd};
  thr.detach();

  std::string input{};
  while (std::getline(std::cin, input)) {
    std::string msg{argv[2]};
    msg.append(": ");
    msg.append(input);

    chat::send_msg(sockfd, msg);
  }

  close(sockfd);

  return 0;
}
