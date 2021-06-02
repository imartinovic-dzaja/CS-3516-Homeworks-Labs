#include "project2.h"
#include "student2.h"
#include <stdlib.h>
#include <stdio.h>

void copyString(char* destination, char* source, int length) {
        for (int i = 0; i < MESSAGE_LENGTH; i++)
                destination[i] = source[i];
}

struct pkt* constructPkt(int seqnum,int acknum,int checksum,char* payload) {
    struct pkt* packetPtr = (struct pkt*) malloc(sizeof(struct pkt));
    packetPtr->seqnum = seqnum;
    packetPtr->acknum = acknum;
    packetPtr->checksum = checksum;
    copyString(packetPtr->payload, payload, MESSAGE_LENGTH);
    return packetPtr;
}

int calculateChecksum(char* vdata,int acknum, int seqnum) {
        int i, checksum = 0;

        for(i = 0; i < MESSAGE_LENGTH; i++){
                checksum += (int)(vdata[i]) * i;
        }

        checksum += acknum * 21;
        checksum += seqnum * 22;
        return checksum;
}

int isCorrupt(struct pkt* packet) {
    if (calculateChecksum(packet->payload, packet->acknum, packet->seqnum) == packet->checksum) {
        return 0;
    }
    return 1;
}





