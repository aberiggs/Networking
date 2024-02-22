#include <arpa/inet.h>
#include <cstddef>
#include <string>
#include <unistd.h>

namespace chat {

void send_msg(int sock_fd, std::string msg) {
  // Send message size
  std::size_t msg_len{htonl(msg.size())};
  send(sock_fd, &msg_len, sizeof(msg_len), 0);

  // Send message
  send(sock_fd, msg.c_str(), msg.size(), 0);
}

std::string receive_msg(int sock_fd) {
  // Fetch message size
  std::size_t msg_len{};
  recv(sock_fd, &msg_len, sizeof(msg_len), 0);
  msg_len = ntohl(msg_len) + 1; // + 1 for null terminator

  // Fetch message
  char *buf = (char *)malloc(sizeof(char) * msg_len);
  buf[msg_len - 1] = '\0';
  recv(sock_fd, buf, msg_len - 1, 0);

  return std::string(buf);
}

} // namespace chat
