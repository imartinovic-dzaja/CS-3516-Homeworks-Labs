#ifndef NODE_COMMON_H_INCLUDED
#define NODE_COMMON_H_INCLUDED

void initDistanceTableWInfinity(int costs[MAX_NODES][MAX_NODES]);
void initDistanceTableWNeighbors(int node, int costs[MAX_NODES][MAX_NODES], struct NeighborCosts* neighbors);
void rtupdate(int node, int costs[MAX_NODES][MAX_NODES], struct RoutePacket* rcvpkt);
void notifyNeighbors(int node, int minCosts[MAX_NODES]);
struct RoutePacket createPacket(int source, int dest, int minCosts[MAX_NODES]);
void printInit(int node);
void printInitCost(int node, struct NeighborCosts* neighbor);
void printCurrentCost(int node, struct RoutePacket* rcvdpkt);
#endif // NODE_COMMON_H_INCLUDED
