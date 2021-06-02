#include "project3.h"
#include <stdio.h>


extern int TraceLevel;
extern float clocktime;

void bellmanFord(int node, int neighbor, int costs[MAX_NODES][MAX_NODES]);
void notifyNeighbors(int node, int minCosts[MAX_NODES]);
struct RoutePacket createPacket(int source, int dest, int minCosts[MAX_NODES]);

//initialize distance table with all infinities
void initDistanceTableWInfinity(int costs[MAX_NODES][MAX_NODES]) {
    for(int i = 0; i < MAX_NODES; ++i) {
        for(int j = 0; j < MAX_NODES; ++j) {
            costs[i][j] = INFINITY;
        }
    }
}

//initialize distance table with neighbor costs
void initDistanceTableWNeighbors(int node, int costs[MAX_NODES][MAX_NODES], struct NeighborCosts* neighbors) {
    for (int i = 0; i < MAX_NODES; ++i) {
        costs[node][i] = neighbors->NodeCosts[i];
    }
}

void rtupdate(int node, int costs[MAX_NODES][MAX_NODES], struct RoutePacket* rcvpkt) {
    int shouldUpdate = NO;
    for(int i = 0; i < MAX_NODES; ++i) {
        if(costs[rcvpkt->sourceid][i] != rcvpkt->mincost[i]) {
            costs[rcvpkt->sourceid][i] = rcvpkt->mincost[i];
            shouldUpdate = YES;
        }
    }
    if (shouldUpdate == YES) {
        bellmanFord(node, rcvpkt->sourceid, costs);
    }
}

void bellmanFord(int node, int neighbor, int costs[MAX_NODES][MAX_NODES]) {
    int should_notify = NO;
    for(int i = 0; i < MAX_NODES; ++i) {
        if(costs[node][i] > (costs[node][neighbor] + costs[neighbor][i])) {
            costs[node][i] = (costs[node][neighbor] + costs[neighbor][i]);
            should_notify = YES;
        }
    }
    if(should_notify==YES) {
        notifyNeighbors(node, costs[node]);
    }
}

void notifyNeighbors(int node, int minCosts[MAX_NODES]) {
    struct NeighborCosts* nCosts = getNeighborCosts(node);
    printf("At time t=%f, node %i current distance vector: %i %i %i %i\n", clocktime, node, minCosts[0], minCosts[1], minCosts[2], minCosts[3]);
    for(int i = 0; i<MAX_NODES; ++i) {
            //if they are neighbors
        if(nCosts->NodeCosts[i] != INFINITY && i!=node) {
            printf("At time t=%f, node %i sends packet to node %i with: %i %i %i %i\n", clocktime, node, i, minCosts[0], minCosts[1], minCosts[2], minCosts[3]);
            toLayer2(createPacket(node, i, minCosts));
        }
    }
}


struct RoutePacket createPacket(int source, int dest, int minCosts[MAX_NODES]) {
        struct RoutePacket sndpkt;
        sndpkt.sourceid = source;
        sndpkt.destid = dest;
        for (int j = 0; j<MAX_NODES; ++j) {
            sndpkt.mincost[j] = minCosts[j];
        }
        return sndpkt;
}

void printInit(int node) {
    if (TraceLevel > 0) {
        printf("At time t=%f, rtinit%i() called.\n", clocktime, node);
    }
}

void printInitCost(int node, struct NeighborCosts* neighbor) {
    if (TraceLevel > 0) {
        printf("At time t=%f, node %i initial distance vector: ", clocktime, node);
            for (int i = 0; i < neighbor->NodesInNetwork; i++) {
                    printf("%i ", neighbor->NodeCosts[i]);
            }
            printf("\n");
    }
}

void printCurrentCost(int node, struct RoutePacket* rcvpkt) {
    if(TraceLevel > 0) {
        printf("At time t=%f, rtupdate%i() called, by a pkt received from Sender id: %i.\n", clocktime, node, rcvpkt->sourceid);
    }
}
