/* Basic client created to learn socket programming */

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int port = 8080;

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    struct sockaddr_in server_address;
    char buffer[16] = { 0 };

    if (argc != 3) {
        std::cerr << "usage: client hostname message\n";
        return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error creating socket.\n";
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }
    
    send(sockfd, argv[2], strlen(argv[1]), 0);
    
    read(sockfd, buffer, sizeof(buffer) - 1);

    std::cout << "Server sent back: " << buffer << "\n";

    close(sockfd);

    return 0;
}