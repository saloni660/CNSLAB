#include <iostream>
#include <string>
#include <bitset>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "crc_utils.h"

std::string binaryToString(const std::string &binary) {
    std::string result = "";
    for (size_t i = 0; i < binary.length(); i += 8) {
        std::bitset<8> bits(binary.substr(i, 8));
        result += char(bits.to_ulong());
    }
    return result;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.50", &addr.sin_addr);

    bind(sockfd, (sockaddr*)&addr, sizeof(addr));

    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    std::cout << "Receiver waiting on 127.0.0.50:12346...\n";

    sockaddr_in sender_addr{};
    socklen_t sender_len = sizeof(sender_addr);
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&sender_addr, &sender_len);

    std::string received_data(buffer);
    std::string key = "1001";

    std::cout << "Received encoded data: " << received_data << std::endl;

    std::string remainder = mod2div(received_data, key);
    if (remainder.find('1') != std::string::npos) {
        std::cout << "❌ Error detected in received data!\n";
    } else {
        std::string data_no_crc = received_data.substr(0, received_data.length() - key.length() + 1);
        std::string original = binaryToString(data_no_crc);
        std::cout << "✅ Data received correctly: " << original << std::endl;
    }

    close(sockfd);
    return 0;
}
