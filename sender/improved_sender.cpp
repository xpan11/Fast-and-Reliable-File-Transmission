#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <netdb.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unistd.h>
using namespace std;

// Define constants instead of magic numbers
//const int UDP_PORT = 36666;
//const int TCP_PORT = 33333;
#define BUF_SIZE 1200
#define WAIT_ON_DELAY 200


class PacketBitmap {
private:
    std::vector<bool> bitmap;

public:
    // Constructor to initialize the bitmap size
    PacketBitmap(size_t num_packets) : bitmap(num_packets, false) {}

    // Mark a packet as received in the bitmap
    void markReceived(size_t packet_number) {
        if (packet_number >= bitmap.size()) {
            std::cerr << "Packet number out of range!" << std::endl;
            return;
        }
        bitmap[packet_number] = true;
    }

    // Check if a packet has been received
    bool isReceived(size_t packet_number) const {
        if (packet_number >= bitmap.size()) {
            std::cerr << "Packet number out of range!" << std::endl;
            return false;
        }
        return bitmap[packet_number];
    }

    // Convert the bitmap to a string for easier debugging
    std::string toString() const {
        std::string result;
        for (const auto& bit : bitmap) {
            result += bit ? '1' : '0';
        }
        return result;
    }
    std::vector<char> serialize() const {
        std::vector<char> serializedBitmap(bitmap.size() / 8 + 1, 0);
        for (size_t i = 0; i < bitmap.size(); ++i) {
            if (bitmap[i]) {
                serializedBitmap[i / 8] |= (1 << (i % 8));
            }
        }
        return serializedBitmap;
    }

    // Deserialize the bitmap
    void deserialize(const std::vector<char>& serializedBitmap) {
        for (size_t i = 0; i < bitmap.size(); ++i) {
            bitmap[i] = serializedBitmap[i / 8] & (1 << (i % 8));
        }
    }

};

void error(const char* msg) {
    perror(msg);
    exit(1); // Consider a different error code for different types of errors.
}

typedef struct packet {
    long int seq;
    long int size;
    char data[BUF_SIZE];
} Packet;

int initialize_udp_socket(const char* ip_address, int UDP_PORT) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");

    struct sockaddr_in reciever;
    bzero(&reciever, sizeof(reciever));
    reciever.sin_family = AF_INET;
    reciever.sin_port = htons(UDP_PORT);
    //reciever.sin_addr.s_addr = inet_addr(ip_address);
    if (bind(sock, (struct sockaddr*)&reciever, sizeof(reciever)) < 0)
        error("binding");

    return sock;
}

int initialize_tcp_socket(const char* ip_address, int TCP_PORT) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("error tcp socket");

    struct sockaddr_in serv_addr;
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip_address);
    serv_addr.sin_port = htons(TCP_PORT);
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("error tcp connectting");
    return sockfd;

    //close(sockfd); // Close the listening socket. You can keep it open if you expect more connections.
    // Return the active socket.
}

void send_packets(int udp_sock, int tcp_sock, const char* filename, const char* ip_address, int UDP_PORT) {
    // (Logic for sending packets, ACKs, etc.)

    FILE* fp = fopen(filename, "rb");
    packet pkt;

    long int pkt_n;
    long int last_packet_size;
    long int file_size;
    if (NULL != fp) {
        // Get the size of the transmit file
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);

        if ((file_size % BUF_SIZE) != 0)
        {
            pkt_n = 1 + (file_size / BUF_SIZE);
        }
        else { pkt_n = file_size / BUF_SIZE; }

        // Send TCP to the other side to announce total packet numbers
        long int nw_pkt_n = ntohl(pkt_n);
        //cout << "send total packet number " << pkt_n << endl;
        int m = write(tcp_sock, &nw_pkt_n, sizeof(nw_pkt_n));
        // Calculate last pakcet size
        if (file_size - BUF_SIZE * (pkt_n - 1) != 0) {
            last_packet_size = file_size - BUF_SIZE * (pkt_n - 1);
        }
        else { last_packet_size = BUF_SIZE; }

    }
    fclose(fp);

    PacketBitmap ack(pkt_n);
    //std::vector<char> tmp_data = ack.serialize();



    //Send out the packet and construct the information
    struct sockaddr_in rec;
    rec.sin_family = AF_INET;
    rec.sin_port = htons(UDP_PORT);
    rec.sin_addr.s_addr = inet_addr(ip_address);

    socklen_t rec_len = sizeof(rec);

    while (1) {
        FILE* fp;
        fp = fopen(filename, "rb");
        int n;

        if (NULL != fp) {
            rewind(fp);

            // Get the data of the file
            char* file_data = new char[file_size + 1];
            fread(file_data, file_size, 1, fp);
            //cout << "last pkt size " << last_packet_size << endl;

            for (int i = 0; i < pkt_n; i++) {
                //Construct each packet's payload and headers
                memset(&pkt, 0, sizeof(pkt));

                //If current packet haven't received by the receiver
                if (!ack.isReceived(i)) {

                    //If current packet is not the last packet
                    if (i != pkt_n - 1) {
                        pkt.seq = i;
                        pkt.size = BUF_SIZE;
                        memcpy(pkt.data, &file_data[i * BUF_SIZE], BUF_SIZE);
                    }

                    //If current packet is the last packet
                    else {
                        pkt.seq = pkt_n - 1;
                        pkt.size = last_packet_size;
                        memcpy(pkt.data, &file_data[file_size - last_packet_size], last_packet_size);
                    }



                    //send out the packet
                    //cout << "pkt.size=" << pkt.size << " pkt.seq=" << pkt.seq << "total_pkt_num="<<pkt_n << endl;

                    n = sendto(udp_sock, &pkt, sizeof(pkt), 0, (struct sockaddr*)&rec, rec_len);


                    usleep(WAIT_ON_DELAY);


                }

            }
            fclose(fp);
            delete[] file_data;





            //Indciate end for one round transmission, waiting for ack
            int end = -1;
            int nw_end = ntohl(end);
            int m = write(tcp_sock, &end, sizeof(end));

            //get reply length(uncertain)
            //int bmLength;
            //read(tcp_sock, &bmLength, sizeof(bmLength));

            int network_len;
            recv(tcp_sock, &network_len, sizeof(network_len), 0);
            //cout << "nw_len=" << network_len << endl;
            int leng = ntohl(network_len);

            //Get the length of the ACK bitmap
            std::vector<char> sBitmap(leng);

            //Get the ACK bit map
            int t_bytes_read = 0;
            char* sBitmap_ptr = sBitmap.data();


            while (t_bytes_read < leng) {
                int bytes_read = recv(tcp_sock, sBitmap_ptr + t_bytes_read, leng - t_bytes_read, 0);

                if (bytes_read == -1) {
                    // Handle errors here
                    break;
                }
                else if (bytes_read == 0) {
                    // The other side has closed the connection
                    break;
                }
                t_bytes_read += bytes_read;
            }

            PacketBitmap temp_ack(pkt_n);

            //Deserialize the bitmap ack
            temp_ack.deserialize(sBitmap);

            //Update received packets informations
            for (int i = 0; i < pkt_n; i++) {
                if (!ack.isReceived(i) && temp_ack.isReceived(i)) {
                    ack.markReceived(i);
                }
            }

        }

    }




}

int main(int argc, char* argv[]) {
    // Add argument checks here
    int udp_port;

    try {
        udp_port = std::stoi(argv[3]);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid udp port number provided." << std::endl;
        return 1;
    }

    int tcp_port;

    try {
        tcp_port = std::stoi(argv[4]);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid tcp port number provided." << std::endl;
        return 1;
    }
    //cout << "udp_port, tcp_port " << udp_port << " " << tcp_port << " start setup proccess, filename "<<argv[2] << endl;

    int udp_sock = initialize_udp_socket(argv[1], udp_port);
    int tcp_sock = initialize_tcp_socket(argv[1], tcp_port);



    send_packets(udp_sock, tcp_sock, argv[2], argv[1], udp_port);



    close(udp_sock);
    close(tcp_sock);
    exit(0);
    return 0;
}