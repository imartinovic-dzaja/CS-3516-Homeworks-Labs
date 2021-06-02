#include <stdio.h>
#include <stdlib.h>
#include "project2.h"
#include <string.h>
#include "student2.h"

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/*
 * The routines you will write are detailed below. As noted above,
 * such procedures in real-life would be part of the operating system,
 * and would be called by other procedures in the operating system.
 * All these routines are in layer 4.
 */

/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */


struct pktQueue** queueA;
StateOfA stateOfA;
StateOfB stateOfB;
int packetSeqNum;



void A_output(struct msg message) {

    //construct packet
    int checksum = calculateChecksum(message.data, packetSeqNum, packetSeqNum);
    struct pkt* sendpkt = constructPkt(packetSeqNum, packetSeqNum, checksum, message.data);

    //enqueue the packet
    enqueue(*queueA, sendpkt);
    //printQueue(*queueA);
    //update packet seq num
    if (packetSeqNum == 0){++packetSeqNum;}
    else {--packetSeqNum;}

    A_sendPkt(*queueA, &stateOfA);

}

/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {

}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {

    if(stateOfA == WAIT_FOR_ACK_ZERO) {
        //If the packet is corrupt or of incorrect acknum
        if(isCorrupt(&packet) || (packet.acknum != 0)) {
            //re-send the packet at beginning of queue
            stopTimer(AEntity);
            tolayer3(AEntity, *((*queueA)->waitingPkt));
            startTimer(AEntity, TIMER_INCREMENT);
        }
        //The packet is not corrupt and of correct acknum
        else {
            //stop the timer
            stopTimer(AEntity);

            //we dequeue the last packet sent
            dequeue(queueA);

            //transition to next state
            stateOfA = WAIT_FOR_CALL_ONE_A;

            //if our queue is not empty attempt sending more packets
            if((*queueA)->waitingPkt != NULL) {
                A_sendPkt(*queueA, &stateOfA);
            }
        }
    }
    else if (stateOfA == WAIT_FOR_ACK_ONE){
        //If the packet is corrupt or of incorrect acknum
        if(isCorrupt(&packet) || (packet.acknum != 1)) {
           //re-send the packet at beginning of queue
            stopTimer(AEntity);
            tolayer3(AEntity, *((*queueA)->waitingPkt));
            startTimer(AEntity, TIMER_INCREMENT);
        }
        //The packet is not corrupt and of correct acknum
        else {
            //stop the timer
            stopTimer(AEntity);

            //we dequeue the last packet sent
            dequeue(queueA);

            //transition to next state
            stateOfA = WAIT_FOR_CALL_ZERO_A;

            //if our queue is not empty send more packets
            if(((*queueA)->waitingPkt) != NULL) {
                A_sendPkt(*queueA, &stateOfA);
            }
        }
    }

    else{
            //do nothing
    }
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
    stopTimer(AEntity);
    tolayer3(AEntity, *((*queueA)->waitingPkt));
    startTimer(AEntity, TIMER_INCREMENT);
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init(){
    stateOfA = WAIT_FOR_CALL_ZERO_A;
    packetSeqNum = 0;

    queueA = (struct pktQueue**) malloc(sizeof (*queueA));
    *queueA = (struct pktQueue*) malloc (sizeof (**queueA));
    (*queueA)->waitingPkt = NULL;
    (*queueA)->next = NULL;
}


/*
 * Note that with simplex transfer from A-to-B, there is no routine  B_output()
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
     struct pkt* sendPkt = (struct pkt*) malloc(sizeof(struct pkt));

    if(stateOfB == WAIT_FOR_CALL_ZERO_B) {
        //If the packet is corrupt or of incorrect seq num
        if(isCorrupt(&packet) || (packet.seqnum != 0)) {
            constructACK(sendPkt, 1);
        }
        //The packet is not corrupt and of correct seqnum
        else {
            //transition to next state
            stateOfB = WAIT_FOR_CALL_ONE_B;

            //pass the data up to layer 5
            struct msg* recvmsg = constructMsg(packet.payload);
            tolayer5(BEntity, *recvmsg);
            free(recvmsg);

            //construct an ACK
            constructACK(sendPkt, 0);
        }
    }
    else {
        //If the packet is corrupt or of incorrect seq num
        if(isCorrupt(&packet) || (packet.seqnum != 1)) {
            constructACK(sendPkt, 0);
        }
        //The packet is not corrupt and of correct seqnum
        else {
            //transition to next state
            stateOfB = WAIT_FOR_CALL_ZERO_B;

            //pass the data up to layer 5
            struct msg* recvmsg = constructMsg(packet.payload);
            tolayer5(BEntity, *recvmsg);
            free(recvmsg);

            //construct an ACK
            constructACK(sendPkt, 1);
        }
    }
    //send the ACK back to A
    tolayer3(BEntity, *sendPkt);
    free(sendPkt);
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void  B_timerinterrupt() {
}

/*
 * The following routine will be called once (only) before any other
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
    stateOfB = WAIT_FOR_CALL_ZERO_B;
}

