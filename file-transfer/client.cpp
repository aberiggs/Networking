/* Basic client modified for sending files */

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int port = 8080;

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    struct sockaddr_in server_address;

    if (argc != 3) {
        std::cerr << "usage: client hostname filename\n";
        return -1;
    }
    
    std::string hostname {argv[1]};
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

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    // Open file
    FILE* file = fopen(argv[2], "rb");
    if (file == nullptr) {
        std::cerr << "Failed to open file\n";
        return -1;
    }   

    // Get and send info about file name
    std::string filename { argv[2] };
    std::size_t start_of_filename {filename.find_last_of("/")};
    if (start_of_filename != std::string::npos) {
        filename = filename.substr(start_of_filename + 1, filename.size());
    }
    std::cout << "Filename of selected file: " << filename << "\n";
    std::size_t filename_length {htonl(filename.size())};
    send(sockfd, &filename_length, sizeof(filename_length), 0);
    send(sockfd, filename.c_str(), filename.size(), 0);

    // Get and send info about size of file
    fseek(file, 0, SEEK_END);
    std::size_t file_size {static_cast<std::size_t>(htonl(ftell(file)))};
    send(sockfd, &file_size, sizeof(file_size), 0);
    fseek(file, 0, SEEK_SET);
    
    char buf[1024] {0};
    std::size_t bytes_read {0};
    while ((bytes_read = fread(buf, sizeof(char), sizeof(buf), file)) > 0) {
        send(sockfd, buf, bytes_read, 0);
    }

    char resp[100] {0};
    recv(sockfd, resp, sizeof(resp), 0);
    resp[99] = '\0';

    std::cout << resp << "\n";

    fclose(file);

    close(sockfd);

    return 0;
}