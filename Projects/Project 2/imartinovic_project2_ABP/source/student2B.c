#include "project2.h"
#include "student2.h"
#include <stdlib.h>
#include <stdio.h>

void constructACK(struct pkt* packet, int acknum) {
    packet->acknum = acknum;
    packet->seqnum = acknum;
    copyString(packet->payload, "01234567890123456789", MESSAGE_LENGTH);
    packet->checksum = calculateChecksum(packet->payload, packet->acknum, packet->seqnum);
}

struct msg* constructMsg(char* data) {
    struct msg* messagePtr = (struct msg*) malloc(sizeof(struct msg));
    copyString(messagePtr->data, data, MESSAGE_LENGTH);
    return messagePtr;
}
