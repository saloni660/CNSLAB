#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>

#define TOTAL_FRAMES 10

using namespace std;

int extract_frame_num(const string& msg) {
    if (msg.find("FRAME:") == 0) {
        return stoi(msg.substr(6));
    }
    return -1;
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.50", &addr.sin_addr);
    bind(sock, (sockaddr*)&addr, sizeof(addr));

    vector<bool> received(TOTAL_FRAMES, false);
    int expected_frame = 0;

    cout << "Receiver started at 127.0.0.50:12346\n";

    while (true) {
        char buffer[1024] = {0};
        sockaddr_in sender_addr{};
        socklen_t len = sizeof(sender_addr);

        int n = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&sender_addr, &len);
        string msg(buffer);

        int frame = extract_frame_num(msg);
        if (frame == -1) continue;

        cout << "Received frame: " << frame << endl;

        // Go-Back-N behavior
        if (frame == expected_frame) {
            received[frame] = true;
            string ack = "ACK:" + to_string(frame);
            sendto(sock, ack.c_str(), ack.size(), 0, (sockaddr*)&sender_addr, len);
            cout << "Sent ACK: " << frame << endl;
            expected_frame++;
        } else if (frame < TOTAL_FRAMES) {
            // For SR: always ACK if not already received
            if (!received[frame]) {
                received[frame] = true;
                string ack = "ACK:" + to_string(frame);
                sendto(sock, ack.c_str(), ack.size(), 0, (sockaddr*)&sender_addr, len);
                cout << "Sent ACK: " << frame << endl;
            }
        }

        if (expected_frame >= TOTAL_FRAMES) {
            cout << "All frames received. Closing.\n";
            break;
        }
    }

    close(sock);
    return 0;
}
