#include "project2.h"
#include "student2.h"
#include <stdlib.h>
#include <stdio.h>

void printQueue(struct pktQueue* pqueue) {
//case of empty queue
    if(pqueue->waitingPkt == NULL) {
        printf("Queue is empty\n");
    }
    //case of non-empty queue
    else {
        struct pktQueue* currentPkt = pqueue;
        printf("%s\n", currentPkt->waitingPkt->payload);
        //loop to reach last message in queue
        while(currentPkt->next != NULL) {
            currentPkt = currentPkt->next;
            printf("%s\n", currentPkt->waitingPkt->payload);
        }
    }

}

void enqueue (struct pktQueue* pqueue, struct pkt* packet) {
    //case of empty queue
    if(pqueue->waitingPkt == NULL) {
        pqueue->waitingPkt = packet;
    }
    //case of non-empty queue
    else {
        struct pktQueue* currentPkt = pqueue;

        //loop to reach last packet in queue
        while(currentPkt->next != NULL) {
            currentPkt = currentPkt->next;
        }
        //at this point the next packet == NULL
        //allocate some memory to store the packet
        currentPkt->next = (struct pktQueue*) malloc(sizeof *(currentPkt->next));
        //hop onto this newly allocated memory for newest packet
        currentPkt = currentPkt->next;
        //fill in its data, setting the next pointer to NULL
        currentPkt->waitingPkt = packet;
        currentPkt->next = NULL;
    }
}


struct pkt* dequeue(struct pktQueue** pqueueDptr) {
    //get our struct
    //case where queue is empty
    if((*pqueueDptr)->waitingPkt == NULL) {
        //nothing to dequeue
        return NULL;
    }
    //case where queue has only one element
    else if((*pqueueDptr)->next == NULL) {
        struct pkt* returnPkt = (*pqueueDptr)->waitingPkt;
        struct pktQueue* temp = *pqueueDptr;
        (*pqueueDptr)->waitingPkt = NULL;
        return returnPkt;
    }
    //case when queue has many elements
    else {
        struct pkt* returnPkt = (*pqueueDptr)->waitingPkt;
        struct pktQueue* temp = *pqueueDptr;
        *pqueueDptr = (*pqueueDptr)->next;
        free(temp);
        return returnPkt;
    }
}

void A_sendPkt(struct pktQueue* pqueue, StateOfA* stateOfA) {
    //if A is ready to send another packet
    if(*stateOfA == WAIT_FOR_CALL_ZERO_A || *stateOfA == WAIT_FOR_CALL_ONE_A) {

        //update the state of A
        if(*stateOfA == WAIT_FOR_CALL_ZERO_A){
            *stateOfA = WAIT_FOR_ACK_ZERO;
        }
        else if (*stateOfA == WAIT_FOR_CALL_ONE_A) {
            *stateOfA = WAIT_FOR_ACK_ONE;
        }
        else {
            printf("\nIncorrect transition of states inside A_output\n");
            exit(1);
        }

        //send the packet from front of queue
        //printf("sending pkt to layerv3\n");
        tolayer3(AEntity, *(pqueue->waitingPkt));

        //start timer
        startTimer(AEntity, TIMER_INCREMENT);
    }
    //otherwise A is waiting for ACK: don't send anything yet
}
