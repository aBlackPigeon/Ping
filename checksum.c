#include "include/checksum.h"

unsigned short compute_checksum(void *buffer,int length){
    unsigned short *buf = buffer;
    unsigned int sum = 0;

    // sum of all words
    while(length > 1){
        sum += *buf++;
        length -= 2;
    }

    // handle last 1 byte
    if(length == 1){
        sum += *(unsigned char*) buf;
    }

    // avoid overflow
    while(sum >> 16){
        sum = (sum & 0xffff) + (sum >> 16);
    }

    // 1s complement
    return (unsigned short)(~sum);
}