/* Modification of a basic server application for usage in a chat app */

#include <algorithm>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "messaging.cpp"

// Port number for the echo server to run on
const int port = 8080;

std::mutex clients_lock{};
std::vector<int> clients{};

void broadcast(std::string msg) {
  std::cout << msg << "\n";
  for (auto fd : clients)
    chat::send_msg(fd, msg);
}

void connection(int client_fd) {
  std::cout << "\nUSER CONNECTED\n\n";
  clients.push_back(client_fd);

  std::string msg = chat::receive_msg(client_fd);
  while (msg != "") {
    std::thread thr{broadcast, msg};
    thr.detach(); // Detach so that we can go back to listening for messages
    msg = chat::receive_msg(client_fd);
  }

  std::vector<int>::iterator it;
  clients_lock.lock();
  it = std::find(clients.begin(), clients.end(), client_fd);
  if (it != clients.end())
    clients.erase(it);
  clients_lock.unlock();

  std::cout << "\nUSER DISCONNECTED\n\n";

  close(client_fd);
}

int main() {
  int server_fd, client_fd;
  sockaddr_in server_address, client_address;
  socklen_t address_size = sizeof(server_address);
  char buffer[16] = {0};

  // Create server socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    std::cout << "Failed to create socket\n";
    return -1;
  }

  // Set up server address
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  // Bind the socket to address/port
  if (bind(server_fd, (sockaddr *)&server_address, sizeof(server_address)) <
      0) {
    std::cout << "Failed to bind socket\n";
    return -1;
  }

  while (true) {
    // Listen for connections
    listen(server_fd, 1);

    // Accept incoming connection
    client_fd = accept(server_fd, (sockaddr *)&client_address, &address_size);

    std::thread thr{connection, client_fd};
    thr.detach(); // Detach so it can run on its own
  }

  close(server_fd);

  return 0;
}
