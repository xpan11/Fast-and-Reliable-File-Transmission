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
using namespace std::chrono;

// Define constants instead of magic numbers
//const int UDP_PORT = 36666;
//const int TCP_PORT = 33333;
#define BUF_SIZE 1200
//if 1ms -> 1000, 200ms -> 200000
#define WAIT_ON_DELAY 200
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



class PacketBitmap {
private:
    std::vector<bool> bitmap;

public:
    // Constructor to initialize the bitmap size
    PacketBitmap(size_t num_packets) : bitmap(num_packets, false) {}

    // Mark a packet as received in the bitmap
    void markReceived(long int packet_number) {
        if (packet_number >= bitmap.size()) {
            std::cerr << "Packet number out of range!" << std::endl;
            return;
        }
        bitmap[packet_number] = true;
    }

    // Check if a packet has been received
    bool isReceived(long int packet_number) const {
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
    //Serilize the bitmap for ack transmission
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


//udp thread
typedef struct {
    int udp_socker;
    struct sockaddr_in client_add;
    PacketBitmap* bitmap;
    const char* filename;

}UdpArgs;

//tcp thread
typedef struct {
    int tcp_socker;
    PacketBitmap* bitmap;

}TcpArgs;


int t_num;
//UDP thread for  receiving file data
void* udp_thread(void* arg) {
    UdpArgs* udp_socket = (UdpArgs*)arg;
    int sock = udp_socket->udp_socker;
    struct sockaddr_in client_addr = udp_socket->client_add;
    PacketBitmap& sharedBitmap = *(udp_socket->bitmap);
    const char* filename = udp_socket->filename;

    socklen_t addr_len = sizeof(client_addr);

    packet pkt;

    //Open file to write
    FILE* fp = fopen(filename, "wb");
    if (!fp) { perror("Fail open file"); }

    int z;
    pthread_mutex_lock(&lock);

    while (t_num > 0) {
        pthread_mutex_unlock(&lock);

        memset(&pkt, 0, sizeof(pkt));

        //Keep receiving if there is still packet missing
        z = recvfrom(sock, &pkt, sizeof(pkt), 0, (struct sockaddr*)&client_addr, &addr_len);
        //cout << "pkt.size=" << pkt.size << " pkt.seq=" << pkt.seq << "still need = " << t_num << " filename= " << filename << endl;

         //Update bitmap with received packet's sequence number
        long int cur = pkt.seq;
        if (!sharedBitmap.isReceived(cur)) {
            char buffer[pkt.size];
            memcpy(buffer, pkt.data, pkt.size);
            fseek(fp, (pkt.seq) * BUF_SIZE, SEEK_SET);
            fwrite(buffer, 1, sizeof(buffer), fp);


            sharedBitmap.markReceived(cur);
            pthread_mutex_lock(&lock);

            //Update missed packet number
            t_num = t_num - 1;
            pthread_mutex_unlock(&lock);

        }


    }
    delete udp_socket;
    fclose(fp);
    return NULL;

}

//TCP thread for network control
void* tcp_thread(void* arg) {
    TcpArgs* tcp_socket = (TcpArgs*)arg;
    int sock = tcp_socket->tcp_socker;
    PacketBitmap& sharedBitmap = *(tcp_socket->bitmap);
    pthread_mutex_lock(&lock);


    while (t_num > 0) {
        pthread_mutex_unlock(&lock);

        //If received end signal -1, then start generating ACK
        int end;
        int m = read(sock, &end, sizeof(end));
        int real_end = ntohl(end);

        if (real_end == -1) {
            pthread_mutex_lock(&lock);

            //Serilize Bitmap ACK
            std::vector<char> data = sharedBitmap.serialize();

            int length = data.size();
            //cout << "actual length = " << length << endl;

            int network_len = htonl(length);
            //cout << "nw_len=" << network_len << endl;

            //First send the length of the ACK bitmap
            m = send(sock, &network_len, sizeof(network_len), 0);
            pthread_mutex_unlock(&lock);

            //Send cumulative ACK in serialized Bitmap
            m = send(sock, data.data(), data.size(),0);

        }
    }
    delete tcp_socket;
    return NULL;

}


int initialize_udp_socket(const char* ip_address, int UDP_PORT) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");

    struct sockaddr_in reciever;
    bzero(&reciever, sizeof(reciever));
    reciever.sin_family = AF_INET;
    reciever.sin_port = htons(UDP_PORT);
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
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(TCP_PORT);
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("error tcp binding");

    listen(sockfd, 5);
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR accept");

    close(sockfd); // Close the listening socket. You can keep it open if you expect more connections.
    return newsockfd; // Return the active socket.
}

void send_packets(int udp_sock, int tcp_sock, const char* filename, const char* ip_address, int UDP_PORT) {
    // (Logic for sending packets, ACKs, etc.)

    struct sockaddr_in rec;
    int leng = sizeof(rec);
    bzero(&rec, leng);
    rec.sin_family = AF_INET;
    rec.sin_port = htons(UDP_PORT);
    rec.sin_addr.s_addr = inet_addr(ip_address);

    long int nw_pkt_num;
    int z = read(tcp_sock, &nw_pkt_num, sizeof(nw_pkt_num));
    long int pkt_num = ntohl(nw_pkt_num);
    t_num = pkt_num;
    //cout << "total packet num = " << pkt_num << endl;
    PacketBitmap* ack = new PacketBitmap(pkt_num);

    pthread_t tcp_threads, udp_threads;

    UdpArgs* udpArgs = (UdpArgs*)malloc(sizeof(UdpArgs));
    udpArgs->udp_socker = udp_sock;
    udpArgs->client_add = rec;
    udpArgs->bitmap = ack;
    udpArgs->filename = filename;

    TcpArgs* tcpArgs = (TcpArgs*)malloc(sizeof(TcpArgs));
    tcpArgs->tcp_socker = tcp_sock;
    tcpArgs->bitmap = ack;

    pthread_create(&udp_threads, NULL, udp_thread, udpArgs);
    pthread_create(&tcp_threads, NULL, tcp_thread, tcpArgs);

    pthread_join(udp_threads, NULL);
    pthread_join(tcp_threads, NULL);

    delete ack;



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

    //cout << "udp_port, tcp_port " << udp_port << " " << tcp_port << " start setup proccess, filename " << argv[2] << endl;

    int udp_sock = initialize_udp_socket(argv[1], udp_port);
    int tcp_sock = initialize_tcp_socket(argv[1], tcp_port);





    send_packets(udp_sock, tcp_sock, argv[2], argv[1], udp_port);
    cout << "*";
    //cout << "finish receving" << " udp_port, tcp_port " << udp_port << " " << tcp_port << endl;

    close(udp_sock);
    close(tcp_sock);
    exit(0);
    return 0;
}