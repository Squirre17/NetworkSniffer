#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/ethernet.h> // ETH_P_ALL
#include <net/if.h> // ETH_P_ALL
#include <netinet/ip.h>   // struct iphdr
#include <netinet/tcp.h>  // struct tcphdr
#include <iomanip>
#include <cstring>


// using namespace std;

void print_hex_dump(const char* payload, int size) {
    const int bytes_per_line = 16;
    const char* hex_digits = "0123456789abcdef";

    // Color codes for highlighting bytes with non-printable characters
    const std::string color_reset = "\033[0m";
    const std::string color_hex = "\033[1;34m"; // blue
    const std::string color_ascii = "\033[1;32m"; // green
    const std::string color_nonprintable = "\033[1;31m"; // red

    for (int i = 0; i < size; i += bytes_per_line) {
        // Print the offset
        std::cout << std::setw(8) << std::setfill('0') << std::hex << i << "  ";

        // Print the hex dump
        for (int j = i; j < i + bytes_per_line; j++) {
            if (j < size) {
                unsigned char byte = static_cast<unsigned char>(payload[j]);
                std::cout << color_hex << hex_digits[byte >> 4] << hex_digits[byte & 0x0f] << " ";
            } else {
                std::cout << "   ";
            }
        }

        // Print the ASCII representation
        std::cout << " |";
        for (int j = i; j < i + bytes_per_line && j < size; j++) {
            unsigned char byte = static_cast<unsigned char>(payload[j]);
            if (std::isprint(byte)) {
                std::cout << color_ascii << byte;
            } else {
                std::cout << color_nonprintable << ".";
            }
        }
        std::cout << color_reset << "|\n";
    }
}

int main() {
    /* 
    htons() is a function in the C programming language that converts a 16-bit unsigned integer 
    from host byte order to network byte order.

    The ETH_P_ALL protocol value is used to capture all traffic passing through the network interface

    AF_PACKET address family is used for creating a packet socket, which is a type of socket that 
    provides direct access to the network interface card (NIC) and allows an application 
    to send and receive packets at the link layer.
    */
    int sock_raw = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
    if(sock_raw < 0) {
        perror("socket:");
        std::cerr << "Failed to create raw socket" << std::endl;
        return -1;
    }

    // Set the socket listening mode
    struct ifreq ifr;
    strncpy(ifr.ifr_name, "ens33", IFNAMSIZ);
    // SIOCGIFFLAGS is used to retrieve the interface flags, which indicate the current state of the interface.
    ifr.ifr_flags = 0;
    ioctl(sock_raw, SIOCGIFFLAGS, &ifr);
    /*
        IFF_PROMISC is a flag that is used in Linux systems to enable promiscuous mode on a network interface.

        SIOCGIFFLAGS => Socket Input Output Control Get Interface Flags
        To retrieve the interface flags using SIOCGIFFLAGS, you would set the ifr_name field of the ifreq structure 
        to the name of the interface you want to query, and set the ifr_flags field to 0
    */
    ifr.ifr_flags |= IFF_PROMISC;
    ioctl(sock_raw, SIOCSIFFLAGS, &ifr);

    while (true) {
        char buffer[65536];
        struct sockaddr saddr;
        int saddr_len = sizeof(saddr);
        int data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_len);
        if(data_size < 0 ) {
            std::cerr << "Failed to receive data" << std::endl;
            break;
        }

        // resolve IP heaer info
        struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
        unsigned short iphdrlen = iph->ihl * 4;

        // resolve TCP/UDP heaer info 
        struct tcphdr *tcph = (struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
        unsigned int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

        // handle payload portion
        char *payload = buffer + header_size;
        int payload_size = data_size - header_size;

        // display relative info
        // inet_ntoa is a C function that converts an IPv4 address in network byte order into a string representation of the address in standard dot notation
        // ntohs is a function in C/C++ programming language that converts a 16-bit number from network byte order to host byte order.
        std::cout << "Source IP: " << inet_ntoa(*(struct in_addr*)&iph->saddr) << std::endl;
        std::cout << "Destination IP: " << inet_ntoa(*(struct in_addr*)&iph->daddr) << std::endl;
        std::cout << "Source Port: " << ntohs(tcph->source) << std::endl;
        std::cout << "Destination Port: " << ntohs(tcph->dest) << std::endl;
        std::cout << "Payload Size: " << payload_size << std::endl;
        std::cout << "Payload Content: " << std::endl;
        print_hex_dump(payload, payload_size);
    }

    // 关闭套接字
    close(sock_raw);
    return 0;
}
