#include <iostream>
#include <string>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "crc_utils.h"

std::string stringToBinary(const std::string &input) {
    std::string binary = "";
    for (char c : input) {
        binary += std::bitset<8>(c).to_string();
    }
    return binary;
}

std::string flipBit(std::string data, int pos) {
    if (pos < 0 || pos >= data.size()) return data;
    data[pos] = (data[pos] == '0') ? '1' : '0';
    return data;
}

int main() {
    std::string input;
    std::cout << "Enter message: ";
    std::getline(std::cin, input);

    std::string binary_data = stringToBinary(input);
    std::string key = "1001";
    std::string encoded = encodeData(binary_data, key);

    int choice;
    std::cout << "\nChoose mode:\n";
    std::cout << "1. Send without error\n";
    std::cout << "2. Send with error (flip 5th bit)\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "Sending data without error...\n";
            break;
        case 2:
            std::cout << "Introducing error in encoded data...\n";
            encoded = flipBit(encoded, 4); // Flip 5th bit (0-indexed)
            break;
        default:
            std::cout << "Invalid choice. Exiting.\n";
            return 1;
    }

    std::cout << "Final Encoded Data: " << encoded << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in recv_addr{};
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.50", &recv_addr.sin_addr);

    sendto(sockfd, encoded.c_str(), encoded.size(), 0, (sockaddr*)&recv_addr, sizeof(recv_addr));
    close(sockfd);

    std::cout << "Data sent to 127.0.0.50:12346\n";
    return 0;
}
