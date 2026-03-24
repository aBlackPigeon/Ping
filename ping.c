#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>


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

    return 0;
}

