Extract the folder imartinovic_project2_ABP.zip
cd to imartinovic_project2_ABP/source
Compile the code by writing "make"	(NOTE: project2.c contains "#include <sys/time.h>" to avoid annoying compilation warning message)
Run the code by writing ./project2

Additional info:
Should be able to handle:
1)Packet corruption
2)Packet loss
3)Randomization

Special notes:
My code handled 99% loss rate with 0 incorrectly received packages out of 10000 messages and an average time between messages of 50 (corruption probability 0)
My code can handle 50% loss rate with an average of 6 incorrectly received packages out of 10000 messages (loss probability 0)

Results from a sample test run:
A sample test run of 10 000 messages 
Loss probability: 90%
Corruption probability: 50%
Out of order probability: 50%
Avg time between messages: 100
Randomized: yes

Results:
Simulator Analysis:
  Number of messages sent from 5 to 4: 10004
  Number of messages received at Layer 5, side A: 0
  Number of messages received at Layer 5, side B: 10000
  Number of messages incorrectly received at layer 5: 5
  Number of packets entering the network: 2319517
  Average number of packets already in network:     0.000
  Number of packets that the network lost: 2087925
  Number of packets that the network corrupted: 115473
  Number of packets that the network put out of order: 115909

But don't take my word for it, try it out yourself!
