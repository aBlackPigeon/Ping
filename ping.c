#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <memory.h>
#include <sys/time.h>
#include "include/checksum.h"

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

    icmp->checksum = compute_checksum(packet,PACKET_SIZE);

    // sending icmp packet

    ssize_t bytes_sent = sendto(sockfd,packet,PACKET_SIZE,0,(struct sockaddr*)&dest_addr,sizeof(dest_addr));
    if(bytes_sent < 0){
        perror("sento failed");
        return 1;
    }

    printf("ICMP Packet Prepared (Header + Timestamp + PAyload)\n");
    printf("Checksum Calculated: 0x%x \n", icmp->checksum);
    printf("Packet sent successfully (%ld bytes)\n", bytes_sent);

    char recv_buffer[1024];

    struct sockaddr_in reply_addr;
    socklen_t addr_len = sizeof(reply_addr);

    ssize_t bytes_received = recvfrom(sockfd,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr*)&reply_addr,&addr_len);
    if(bytes_received < 0){
        perror("recvfrom error");
        return 1;
    }

    printf("Packets received (%ld bytes)\n", bytes_received);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&reply_addr.sin_addr,ip_str,sizeof(ip_str));

    printf("Reply from %s\n", ip_str);

    struct iphdr *ip = (struct iphdr*)recv_buffer;
    int ip_header_len = ip->ihl * 4;

    struct icmphdr *icmp_reply = (struct icmphdr*)(recv_buffer + ip_header_len);

    if(icmp_reply->type == ICMP_ECHOREPLY && icmp->un.echo.id == getpid()){
        printf("Valid Icmp echo replied received\n");
        printf("Sequence: %d\n", icmp_reply->un.echo.sequence);

        // extract timestamp

        struct timeval *time_sent = (struct timeval*)(recv_buffer + ip_header_len + sizeof(struct icmphdr));

        struct timeval time_received;
        gettimeofday(&time_received,NULL);

        // rtt
        double rtt;
        
        rtt = (time_received.tv_sec - time_sent->tv_sec) * 1000.0;
        rtt += (time_received.tv_usec - time_sent->tv_usec) / 1000.0;

        printf("RTT: %.2f ms\n", rtt);

    }else{
        printf("Ignored Packet\n");
    }

    return 0;
}

