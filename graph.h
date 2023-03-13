#ifndef JUSTINTIMEROUTING_GRAPH_H
#define JUSTINTIMEROUTING_GRAPH_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <chrono>
#include "heap.h"
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <queue>
#include <numeric>
//#include <boost/heap/fibonacci_heap.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
#include "math.h"
#include <boost/heap/fibonacci_heap.hpp>
#include "linearPiecewiseFunction.h"
//#include "structures.h"

using namespace std;
using namespace benchmark;

typedef struct COMPARENODE{
    pair<int, int> pif; // ID, minCost
    bool operator() (const struct COMPARENODE& a, const struct COMPARENODE& b) const
    {
        return a.pif.second > b.pif.second;
    }
}compareNode;

struct Node{
    int nodeID;
    double lng, lat;
};

struct Edge{
    LPFunction costFunction;
    int ID1, ID2, length, edgeID;
    
};

struct FPResult {
    vector<int> path;
    vector<int> pathEdge;
    vector<int> vTime;

    FPResult* parent = nullptr;

    // Test the size of three variables
    bool integrality() {
        int n = path.size();
        int n1 = pathEdge.size();
        return (n1 == n - 1) && (vTime.size() == n);
    }


    // Concatenate sub-paths
    void joint(FPResult& fpr) {
        if (!integrality() || !fpr.integrality()) {
            std::cout << "The FPResults have the wrong format.\n";
            exit(-1);
        }

        int n = path.size();
        if (!path[n - 1] == fpr.path[0]){ //&& pathEdge[n-2]==fpr.path[0] //&& vTime[n-1]==fpr.vTime[0]
            std::cout << "These two FPResults cannot be joints.\n";
            exit(-2);
        }

        for (int i = 0; i < fpr.pathEdge.size(); ++i) {
            path.emplace_back(fpr.path[i + 1]);
            pathEdge.emplace_back(fpr.pathEdge[i]);
            vTime.emplace_back(fpr.vTime[i + 1]);
        }
    }

    void split(int n) {
        path.resize(n);
        pathEdge.resize(n - 1);
        vTime.resize(n);
    }

    int getTravelTime() const {
        return vTime[vTime.size() - 1] - vTime[0];
    }

    const bool operator < (const FPResult& rhs) const {
        return getTravelTime() < rhs.getTravelTime();
    }
};

struct pathNode {
    int rs = 0; // node
    int rt = 0; // timestamp
    // int d = 0;
    int layer = -1;
    FPResult fpr;
    int numson = 0;
    std::vector<pathNode*> children;
    pathNode* nextPath = nullptr;
    pathNode() = default;
    pathNode(int s_, int t_) :rs(s_), rt(t_) {}
};


struct gQuery
{
    int qID;
	int ID1, ID2, t;
    double direction;
    int x1, y1; // grid coordinate of ID1
    int x2, y2; // grid coordinate of ID2
    int ed; // euclidean distance between ID1 and ID2
    int ClusterID;
    double threshold = 0.9;
    int k = 5;
	gQuery() = default;
	gQuery(int s_, int e_, int t_) :ID1(s_), ID2(e_), t(t_) {
		//rs = ID1;
		//rt = t;
	}
    int cl = 0; // current layer;
    int arriveLayer = -1;

    std::vector<int> layerTimestamp;
    pathNode* root = nullptr;
    pathNode* curPath = nullptr;
    FPResult finalPath;
    //int curLayer = 0;
    //int nChain = -1;
    //std::vector<pathNode*> chosenChain;
    double qtime = 0.0;

    bool isReach() {
        if (curPath) {
            auto& p = curPath->fpr.path;
            return p[p.size() - 1] == ID2;
        }

        std::cout << "Error: SPT is not built\n";
        exit(-1);
    }

    void jointPath() {
        pathNode* node = root;
        finalPath = node->fpr;
        while (1) {
            node = node->nextPath;
            if (node)
                finalPath.joint(node->fpr);
            else
                break;
        }
        auto& p = finalPath.path;
        if (!(p[p.size() - 1] == ID2)) {
            std::cout << "Error: The final path does not reach the ID2! \n";
        }
    }


    void findOptPath() {
        minpathCost(curPath);
    }

    int minpathCost(pathNode* node) {
        if (!node) return 0;
        int cc = 0;
        std::vector<int> sls(node->numson, -1);
        int id_min = -1;
        int cc_min = 10000000;
        for (int i = 0; i < node->numson; ++i) {
            sls[i] = minpathCost(node->children[i]);
            //int sl = minLayer(node->children[i]);
            if (node->children[i]) {
                if (sls[i] < cc_min) {
                    cc_min = sls[i];
                    id_min = i;
                }
            }
        }
        if (id_min == -1) {
            auto& p = node->fpr.path;
            if (p[p.size() - 1] == ID2) {
                return node->fpr.getTravelTime();
            }
            else {
                return 10000000;
            }
        }
        else {
            node->nextPath = node->children[id_min];
            return node->fpr.getTravelTime() + cc_min;
        }
    }

    int maxLayer(pathNode* node) {
        if (!node) return 0;
        int sml = 0;
        for (int i = 0; i < node->numson; ++i) {
            int sl = maxLayer(node->children[i]);
            if (sml < sl) sml = sl;

            //sls[i] = minLayer(node->children[i]);
            ////int sl = minLayer(node->children[i]);
            //if (node->children[i])
            //    if (sml > sls[i]) sml = sls[i];
        }
        //if (node->numson == 0) return 1;
        return sml + 1;
    }

    int minLayer(pathNode* node) {
        if (!node) return 0;
        int sml = 10000000;
        std::vector<int> sls(node->numson, -1);
        for (int i = 0; i < node->numson; ++i) {
            sls[i] = minLayer(node->children[i]);
            //int sl = minLayer(node->children[i]);
            if (node->children[i])
                if (sml > sls[i]) sml = sls[i];
        }
        if (sml == 10000000) return 1;
        return sml + 1;
    }

    int size(pathNode* node) {
        if (!node) return 0;
        int s = 1;
        //std::vector<int> sls(node->numson, -1);
        for (int i = 0; i < node->numson; ++i) {
            s+=size(node->children[i]);
        }
        return s;
    }

    void showInfo(gQuery& gq) {
        std::cout << "The timeSlot is:    " << layerTimestamp.size() << std::endl
//            << "The maxLayer is:     " << maxLayer(root) << std::endl
//            << "The minLayer is:     " << minLayer(root) << std::endl
//            << "The arriveLayer is:  " << arriveLayer << std::endl
            << "The size of tree is:  " << size(root) << std::endl
            << std::endl;

//        string path = "data/usefulqrs";
//        std::ofstream out(path.c_str(), std::ios::binary | std::ios::app);
//        //out << numV << " " << xmin << " " << xmax << " " << ymin << " " << ymax << "\n";
//        out << gq.ID1 << " " << gq.ID2 << " " << gq.t << "\n";
//        out.close();
    }

};

// ellipse
typedef struct ellCluster{
    int clusterID;
    set<pair<int, int> > sQuerySorted;
    set<int> sQuery;  // covered queries (id)
    bool bF;
    int d;
    double direction;

    // The grids this ellipse covers
    set<pair<int, int> > sGrid;

    bool operator < (const ellCluster c) const
    {
        if (sQuery.size() == c.sQuery.size())
            return clusterID < c.clusterID;
        return sQuery.size() < c.sQuery.size();
    }
}ellCluster;



class Graph{
public:
    Graph() = default;
    Graph(string filename){
        readRoad(filename);
    }

    int nodeNum;
    int edgeNum;
    double maxLong, maxLat, minLong, minLat; // the coordinate range of the entire graph
    int upperbound = 7200; // Porto data

    int dT = 1000;
    int k = 5;
    int uT = 20;
    double threshold = 0.9;

    string spfile_suf = ".txt";
    string spfile_pre =  "protosp";

    vector<vector<int>> adjList;  // neighborID, Distance
    vector<vector<int>> adjListR;
    vector<vector<pair<int, int> > > adjListEdge;	// neighborID, edgeID
    vector<vector<pair<int, int> > > adjListEdgeR;
    vector<vector<pair<int, int> > > adjListCost;	//neighborID, cost
    vector<vector<pair<int, int> > > adjListCostR;

    vector<vector<int> > dNode;

    vector<Node> vNode;
    vector<Edge> vEdge;
    vector<Edge> vEdgeR;

    vector<pair<double, double> > vCoor;
    unordered_map<string, pair<int, int> > mCoor;

    vector<gQuery> Queries;
    //x, y, pair<coherence value, road number>
    vector<vector<pair<double, int> > > vvCoherence;
    //x, y, set<edge id>
    vector<vector<set<int > >> vvGridEdge;
    //determine the decomposition granularity
    int power = 6;
    vector<int> vBase;
    pair<double, double> vpXYBase;
    vector<vector<set<int> > > vvsGridF;
    vector<vector<set<int> > > vvsGridB;
    vector<ellCluster> vCluster;

    // angle computation
    double Angle(Node s, Node t)
    {
        double lat = t.lat - s.lat;
        double lng = t.lng - s.lng;
        double absAngle = atan(abs(1.3267*lat/lng));
        double trueAngle = 0;
        if(lat >= 0 && lng > 0)
            trueAngle = absAngle;
        else if(lat > 0 && lng <= 0)
            trueAngle = PI - absAngle;
        else if(lat <= 0 && lng < 0)
            trueAngle = PI + absAngle;
        else if(lat < 0 && lng >= 0)
            trueAngle = 2*PI - absAngle;

        return trueAngle;
    }

    int readRoad(string filename);
    int readMap(string filename);
    int readSpeedProfile(string filename);
    int readQuery(string filename);
    bool allEqual(vector<pair<int, LPFunction> >& vpOut);
    int getPos(vector<int>& vX, int vx);
    int getvP(int vx, LPFunction& lpf);
    struct FPResult getPath(vector<pair<int, LPFunction>>& vpOut, int depTime, int ID1, int ID2);
    int DeviationCost(int depTime, int eID, vector<pair<int, LPFunction> >& vpOut);
    void PathDeviationCost(benchmark::pHeap<3, int, int, int>& Arc, int eID1Pos, int pID, vector<int> sp,
                           vector<int> spedge, vector<int> sptime, vector<pair<int, LPFunction> >& vpOut, int mineID);
    vector<pair<int, struct LPFunction>> BackwardTDFCat(int sourceID, int ID1);
    vector<int> LCSS(vector<int> &p1, vector<int> &p2);
    FPResult SSFP(int ID1, int ID2, int t);
    void FindRepeatedPath(vector<vector<int> >& vvPath);

    void initCoherence();
    void createCoherence();
    int Eucli(double lng1, double lat1, double lng2, double lat2);
    void coveredGrids(double x1, double y1, double x2, double y2, double constDist, set<pair<int, int> >& sGrids);
    set<pair<int, int> > ellipseDecompose(gQuery& gq);
    double shortestPathError(int &d1, int &d2);
    void addAllChildren(int x, int y, set<pair<int, int> >& sGrids);
    void RegionDecomposition();
    void showCluster();

};


#endif // JUSTINTIMEROUTING_GRAPH_H
