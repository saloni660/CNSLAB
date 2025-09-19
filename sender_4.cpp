#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10

using namespace std;

void send_frame(int sock, sockaddr_in &receiver, int frame_num, bool simulate_loss) {
    string message = "FRAME:" + to_string(frame_num);
    if (simulate_loss) {
        cout << "Simulating loss of frame " << frame_num << endl;
        return;
    }
    sendto(sock, message.c_str(), message.length(), 0, (sockaddr*)&receiver, sizeof(receiver));
    cout << "Sent frame: " << frame_num << endl;
}

void go_back_n(int sock, sockaddr_in &receiver) {
    int base = 0;

    while (base < TOTAL_FRAMES) {
        for (int i = 0; i < WINDOW_SIZE && (base + i) < TOTAL_FRAMES; ++i) {
            send_frame(sock, receiver, base + i, false);
        }

        char ack[1024];
        socklen_t len = sizeof(receiver);
        recvfrom(sock, ack, sizeof(ack), 0, (sockaddr*)&receiver, &len);
        string ack_msg(ack);
        
        if (ack_msg.find("ACK:") != string::npos) {
            int ack_num = stoi(ack_msg.substr(4));
            cout << "Received ACK for frame " << ack_num << endl;

            if (ack_num == base + WINDOW_SIZE - 1) {
                base += WINDOW_SIZE;
            } else {
                cout << "Go-Back-N: ACK mismatch, resending from frame " << base << endl;
                // Simulate Go-Back-N behavior
                usleep(1000000);
            }
        }
    }
}

void selective_repeat(int sock, sockaddr_in &receiver) {
    vector<bool> acked(TOTAL_FRAMES, false);
    int base = 0;

    while (base < TOTAL_FRAMES) {
        for (int i = 0; i < WINDOW_SIZE && (base + i) < TOTAL_FRAMES; ++i) {
            if (!acked[base + i]) {
                bool simulate_loss = ((base + i) == 2 || (base + i) == 5);  // Simulate loss
                send_frame(sock, receiver, base + i, simulate_loss);
            }
        }

        // Receive acks
        for (int i = 0; i < WINDOW_SIZE; ++i) {
            char ack[1024] = {0};
            socklen_t len = sizeof(receiver);
            int n = recvfrom(sock, ack, sizeof(ack), MSG_DONTWAIT, (sockaddr*)&receiver, &len);
            if (n > 0) {
                string ack_msg(ack);
                if (ack_msg.find("ACK:") != string::npos) {
                    int ack_num = stoi(ack_msg.substr(4));
                    acked[ack_num] = true;
                    cout << "Received ACK for frame " << ack_num << endl;
                }
            }
        }

        // Slide window
        while (base < TOTAL_FRAMES && acked[base]) {
            base++;
        }

        usleep(1000000);
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in receiver{};
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.50", &receiver.sin_addr);

    int choice;
    cout << "Select Protocol:\n1. Go-Back-N\n2. Selective Repeat\nEnter choice: ";
    cin >> choice;

    switch (choice) {
        case 1:
            go_back_n(sock, receiver);
            break;
        case 2:
            selective_repeat(sock, receiver);
            break;
        default:
            cout << "Invalid choice\n";
            return 1;
    }

    close(sock);
    return 0;
}
