/* Basic server modified for receiving files  */

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

        
        
        FILE* file = fopen("storage/transfer.txt", "wb");
        if (file == nullptr) {
            std::cout << "Failed to open file for writing\n";
            std::string resp {"Server: Failure writing file"};
            send(client_fd, resp.c_str(), resp.size(), 0);
            return -1;
        }

        char buf[1024] {0};
        std::size_t bytes_read {0};
        while ((bytes_read = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
            std::cout << "Receiving " << bytes_read << " bytes\n";
            fwrite(buf, sizeof(char), bytes_read, file);
        }

        std::cout << "Finished writing\n";

        fclose(file);

        std::string resp {"Server: Success!"};
        send(client_fd, resp.c_str(), resp.size(), 0);

        // Close open file descriptors
        close(client_fd);
    }

    close(server_fd);

    return 0;
}