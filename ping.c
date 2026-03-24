#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <memory.h>
#include <sys/time.h>

#define PACKET_SIZE 64
#define DATA_SIZE (PACKET_SIZE - sizeof(struct icmphdr))

int main(int argc , char *argv[]){

    if(argc != 2){
        printf("Usage : %s <IP_ADDRESS> ", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];

    int sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(sockfd < 0){
        perror("Socket Creation failed");
        return 1;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = 0;

    if(inet_pton(AF_INET,target_ip,&dest_addr.sin_addr) <= 0){
        printf("Invalid Address");
        return 1;
    }

    printf("Raw Socket Created Successfully\n");
    printf("Target IP : %s\n", target_ip);

    // ip Packet

    char packet[PACKET_SIZE];
    memset(packet,0,sizeof(packet));

    struct icmphdr *icmp = (struct icmphdr*) packet;

    static int sequence = 1;

    icmp->type = 8;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.id = getpid();
    icmp->un.echo.sequence = sequence++;

    char *data = packet + sizeof(struct icmphdr);
    
    // insert timestamp
    struct timeval *time_sent = (struct timeval *) data;
    gettimeofday(time_sent,NULL);

    // filling payload
    memset(data + sizeof(struct timeval), 'A', DATA_SIZE - sizeof(struct timeval));
    // packet look liks
    // ICMP HEADER
    // struct timeval
    // Padding (A's)

    printf("ICMP Packet Prepared (Header + Timestamp + PAyload)\n");

    return 0;
}

