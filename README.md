# Description

Travel Time Constrained Top-k^n Shortest Path Algorithm is stored in TTCknSP.h:
1. eTDKSPCompare function performs the k^n paths preparation for queries;

2. selectResult function performs the continuous routing process for queries;


A test dataset is stored in the folder TTCKnSP/cmake-build-debug/data/:
1. portocoords.txt and portoroad.txt provide roadnetwork information;

2. portosp_gt.txt is a concatenated ground truth speed profile;

3. portosp0.txt is an initial speed profile obtained by a single time of prediction;

4. portosp1-7.txt are concatenated speed profile obtained by multiple times of prediction. 


Please run main.cpp to watch the test result. The running result shows the following information:  
1. the preparation process of top-k^n paths within layers;

2. the final path concateantion for all queries under the region-based speed profile update;

3. the shortest path error compared to the result performed on the ground truth speed profile.
