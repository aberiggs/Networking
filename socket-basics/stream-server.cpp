/* Basic echo server created to learn socket programming */

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Port number for the echo server to run on
const int port = 8080;

// The max number of queued connections allowed
const int backlog = 5;

int main() {
    int server_fd, client_fd;
    sockaddr_in server_address, client_address;
    socklen_t address_size = sizeof(server_address);
    char buffer[16] = { 0 };

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
    if (bind(server_fd, (sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cout << "Failed to bind socket\n";
        return -1;
    }

    while (true) {
        // Listen for connections
        listen(server_fd, backlog);

        // Accept incoming connection
        client_fd = accept(server_fd, (sockaddr *)&client_address, &address_size);

        // Read message from client
        read(client_fd, buffer, sizeof(buffer)-1);
        // Ensure null terminator exists at end of string
        buffer[sizeof(buffer)-1] = '\0';

        std::cout << "Client sent: " << buffer << "\n";

        // Echo back to client
        send(client_fd, buffer, strlen(buffer), 0);

        // Close open file descriptors
        close(client_fd);
    }

    close(server_fd);

    return 0;
}