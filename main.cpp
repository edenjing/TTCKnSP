#include "graph.h"
#include "TTCknSP.h"

int main()
{
    std::string dir = "data/";
    string roadfile = dir + "portoroad.txt";
    string gtspfile = dir + "portosp_gt.txt";
    string spfile = dir + "portosp0.txt";
    string spfile_pre = dir + "portosp";
    string spfile_suf = ".txt";
    string queryfilename = dir + "portoquery.txt";
    string mapfile = dir + "portocoords.txt";

    pmGraph g = pmGraph();
    g.readRoad(roadfile);
    g.readMap(mapfile);
    g.readSpeedProfile(spfile);
    cout << "Graph loading finish." << endl;
    g.readQuery(queryfilename);

    g.initCoherence();
    // Construct vvCoherence: the road number in each grid
    // Construct vvGridEdge: the edge id in each grid
    g.createCoherence();
    // Construct vCluster (which contains sGrid): the covered grids in each cluster (query)
//    g.RegionDecomposition();
//    g.showCluster();

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;

//    vector<double> eTDKSPCompareTime;
//    vector<int> eTDKSPCompareAverageLength;

    int k = 5; // top-k path number
    double t = 0.9; // similarity threshold
    int dT = 2000; // \delta T
    int uT = 20; // sp update frequency

    g.k = k;
    g.dT = dT;
    g.uT = uT;
    g.threshold = t;
    g.spfile_pre = spfile_pre;
    g.spfile_suf = spfile_suf;

    cout << "---------------------------------------------------------" << endl;
    cout << "Perform routing..." << endl;
    cout << "---------------------------------------------------------" << endl;

    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < g.Queries.size(); ++i) {
        cout << "Prepare k^n paths for query " << i << "..." << endl;
        g.eTDKSPCompare(g.Queries[i]);
        g.Queries[i].showInfo(g.Queries[i]);
    }
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    cout << "Average k^n path preparation time: " << time_span.count() / g.Queries.size() << "s" << endl;
    cout << endl;

    t1 = std::chrono::high_resolution_clock::now();
    g.selectResult(g.Queries);
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    cout << "Average path selection time: " << time_span.count() / g.Queries.size() << "s" << endl;

    // Shortest Path Error
    vector<int> continuousCost;
    vector<int> actualCost;

    g.readSpeedProfile(gtspfile);

    for(int i = 0; i < g.Queries.size(); i++){
        // Travel Cost of Continuous Optimal Path
        continuousCost.push_back(g.Queries[i].finalPath.getTravelTime());
        // Travel Cost of Actual Optimal Path
        FPResult ap;
        ap = g.SSFP(g.Queries[i].ID1, g.Queries[i].ID2, g.Queries[i].t);
        actualCost.push_back(ap.vTime[ap.vTime.size()-1] - ap.vTime[0]);
    }

    vector<double> pathError;
    for(int j = 0; j < continuousCost.size(); j++){
        pathError.push_back(g.shortestPathError(continuousCost[j], actualCost[j]));
    }
    double avgError = 0.0;
    avgError = accumulate(pathError.begin(), pathError.end(), 0.0) / pathError.size();
    cout << "Average shortest path error: " << (double)avgError << endl;
    return 0;
}

void Graph::FindRepeatedPath(vector<vector<int> >& vvPath)
{
    for(int i = 0; i < (int)vvPath.size()-1; i++){
        vector<int> vSame;
        for(int j = i + 1; j < (int)vvPath.size(); j++){
            if(vvPath[i].size() != vvPath[j].size())
                continue;

            bool bSame = true;
            for(int k = 0; k < vvPath[i].size(); k++){
                if(vvPath[i][k] != vvPath[j][k]){
                    bSame = false;
                    break;
                }
            }
            if(bSame)
                vSame.push_back(j);
        }

        if(!vSame.empty()){
            cout << "Same path of " << i << ":" << endl;
            for(auto& sID : vSame)
                cout << sID << "\t";
            cout << endl;
        }
    }
}
