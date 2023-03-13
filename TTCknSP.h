#pragma once

/**********************************************************/
// Travel Time Constrained Top-k^n Path Algorithm
/*********************************************************/

#include "graph.h"

struct event {
    int t = 0;
    int qid = -1;

    event() = default;
    event(int t_, int qid_) : t(t_), qid(qid_) {}

    const bool operator<(const event& re) const {
        return t < re.t;
    }
};

class pmGraph : public Graph
{
public:

    int lastUT = 0; // last update time
//    int oldUT = -1;

    int spid = 0;

	// Prepare top-k shortest paths
	void processLayer(gQuery& gq, pathNode* root) {
        //auto& root = gq.root;
        auto& ID1 = root->rs;
        auto& ID2 = gq.ID2;
        if (ID1 == ID2){
            cout << "ID1 == ID2 (in processLayer)." << endl;
            return;
        }
        auto& DPT = root->rt;
        // auto& k = this->k;
        auto& t = gq.threshold;

        std::chrono::high_resolution_clock::time_point t1; 
        std::chrono::high_resolution_clock::time_point t2;
        std::chrono::duration<double> time_span;

        // Backward Concatenation
        t1 = std::chrono::high_resolution_clock::now();
        vector<pair<int, LPFunction> > vpOut = BackwardTDFCat(ID2, ID1);
        t2 = std::chrono::high_resolution_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        cout << "Backward Concatenation Time (in processLayer):" << time_span.count() << endl;
        allEqual(vpOut);

        // Get initial path in current layer
        FPResult initialSP = getPath(vpOut, DPT, ID1, ID2);

        // Equal to "if (ID1 == ID2) return";
        if (initialSP.path.size() == 1) {
            cout << "No initial path generated (in eTDKSPCompare)." << endl;
            return;
        }

        vector<int> kResults;
        vector<vector<int>> vkPathNode;
        vector<vector<int>> vkPathEdge;
        vector<vector<int>> vkPathTime;
        int countNumber = -1;
        bool bCountNumber = true;

        vector<vector<int>> vvResult; // Exact Paths
        vvResult.reserve(k);
        //    vector<vector<int>> vvNodeResult;
        //    vvNodeResult.reserve(k);
        vector<int> vDistance;
        vector<int> vPathParent; // deviated from
        vector<int> vPathParentPos;	// deviated Pos from Parent
        vector<int> vPathDeviation; // deviation node
        vector<int> vPathDevPrime; // The other node of the deviation edge
        vector<vector<int> > vvPathCandidate; // nodes
        vector<vector<int> > vvPathCandidateEdge; // edges
        vector<vector<int>> vvPathCandidateTime; // departure times
    //    vector<unordered_map<int, int> > vmPathNodePos; // Position of the non-fixed vertices
    //    vector<multimap<int, int> > vmArc;
        vector<vector<pair<int, int>>> vvmPathNodePos;
        vector<vector<pair<int, int>>> vvmArc;
        vector<int> vFather;

        vPathDevPrime.reserve(nodeNum * 10);
        vPathDeviation.reserve(nodeNum * 10);

        vector<unordered_set<int>> pResult;
        vFather.push_back(-1);

        float sim = 0.0f;
        benchmark::pHeap<3, int, int, int> Arc(2 * nodeNum);

        vector<int> vPath;
        vector<int> vPathEdge;
        vector<int> vPathTime;
        vPath = initialSP.path;
        vPathEdge = initialSP.pathEdge;
        vPathTime = initialSP.vTime;

        /*multimap<int, int> mArc;
        // Initial mineID is set as -1
        mArc = PathDeviationCost(vPath, vPathEdge, vPathTime, vpOut, -1);
        cout << "mArc.size(): " << mArc.size() << endl;
        vmArc.push_back(mArc); // vector<multimap<int, int> > vmArc;*/
        // initial mineID, pID and eID1Pos are set as -1
        PathDeviationCost(Arc, -1, -1, vPath, vPathEdge, vPathTime, vpOut, -1);

        int mmArcEdgeID, mmArcReducedLength, mmArcPosition;
        vector<pair<int, int>> mmArc;
        vector<pair<int, int>> mmPos;
        while (!Arc.empty()) {
            Arc.extract_min(mmArcEdgeID, mmArcReducedLength, mmArcPosition);
            mmArc.push_back(make_pair(mmArcEdgeID, mmArcReducedLength));
            mmPos.push_back(make_pair(mmArcEdgeID, mmArcPosition));
        }
        vvmArc.push_back(mmArc);
        vvmPathNodePos.push_back(mmPos);

        /*unordered_map<int, int> mPos;
        for(int i = 0; i < (int)vPath.size(); i++) // Store the positions of all nodes in vPath sequentially
            mPos[vPath[i]] = i;
        vmPathNodePos.push_back(mPos);*/ // Position of the non-fixed vertices; vector<unordered_map<int, int> > vmPathNodePos;

        benchmark::heap<2, int, int> qPath(nodeNum);
        vvPathCandidate.push_back(vPath);
        vvPathCandidateEdge.push_back(vPathEdge);
        vvPathCandidateTime.push_back(vPathTime);

        //    vDistance.push_back(vSPTDistance[ID1]);
        // total travel time of the initial path, from ID1 to ID2
        vDistance.push_back(vPathTime[vPathTime.size() - 1] - vPathTime[0]);

        vPathParent.push_back(-1); // deviated from
        vPathParentPos.push_back(0); // deviated Pos from parent
        vPathDeviation.push_back(ID1); // deviation node

    //    qPath.update(vvPathCandidate.size()-1, vSPTDistance[ID1]);
        qPath.update(vvPathCandidate.size() - 1, vPathTime[vPathTime.size() - 1] - vPathTime[0]);

        vector<int> vResultID;
        int topPathID, topPathDistance;
        int oldDistance = -1;
        while ((int)kResults.size() < k && !qPath.empty())
        {
            //        cout << "kResults.size(): " << kResults.size() << endl;
            float addLength = 0;
            // extract_min(): Remove and return the element with the smallest keyword
            qPath.extract_min(topPathID, topPathDistance);
            //        cout << "topPathID: " << topPathID << "\t" << "topPathDistance:" << topPathDistance << endl;

            if (topPathDistance < oldDistance) {
                cout << "Error" << endl;
            }
            oldDistance = topPathDistance;
            // Loop Test: if there is a loop, don't put it in kResult
            unordered_set<int> us;
            bool bTopLoop = false;
            for (auto& v : vvPathCandidate[topPathID]) {
                if (us.find(v) == us.end())
                    us.insert(v);
                else {
                    bTopLoop = true;
                    break;
                }
            }
            //if (bTopLoop) continue;
            if (!bTopLoop) // if no loop
            {
                int n = 0;
                if (vvResult.size() == 0)  // put the initial shortest path
                {
                    vvResult.push_back(vvPathCandidateEdge[topPathID]);
                    kResults.push_back(topPathDistance);
                    vkPathNode.push_back(vvPathCandidate[topPathID]);
                    vkPathEdge.push_back(vvPathCandidateEdge[topPathID]);
                    vkPathTime.push_back(vvPathCandidateTime[topPathID]);

                    vResultID.push_back(topPathID);
                    unordered_set<int> pTmp;
                    //auto& pTmp = us;
                    for (auto ie = vvPathCandidateEdge[topPathID].begin();
                        ie != vvPathCandidateEdge[topPathID].end(); ie++) {
                        pTmp.insert(*ie);
                    }
                    pResult.push_back(pTmp);
                }
                else
                {
                    for (int i = 0; i < vResultID.size(); i++) // vResultID: store the pathID of all results
                    {
                        //                  countNonAncestor += 1;
                        for (auto ie = vvPathCandidateEdge[topPathID].begin();
                            ie != vvPathCandidateEdge[topPathID].end(); ie++) {
                            if (pResult[i].find(*ie) != pResult[i].end())
                                addLength += vEdge[*ie].length;
                        }
                        //Sim 1: Jaccard Similarity
                        //sim = addLength / (kResults[i] + topPathDistance - addLength);

                        //Sim 2: Arithmetic Average
                        //sim = addLength / (2*kResults[i]) + addLength / (2*topPathDistance);

                        //Sim 3: Overlap Ratio Min
                        /*int minLength;
                        if(kResults[i] < topPathDistance)
                            minLength = kResults[i];
                        else
                            minLength = topPathDistance;
                        sim = addLength / minLength;*/

                        //Sim 4: LCSS
                        /*int lcssLength;
                        vector<int> lcssEdge;
                        vector<int> pR;
                        pR.assign(pResult[i].begin(), pResult[i].end()); // convert unordered_set to vector
                        lcssEdge = LCSS(pR, vvPathCandidateEdge[topPathID]);
                        for(auto ie = lcssEdge.begin(); ie != lcssEdge.end(); ie++){
                            lcssLength += vEdge[*ie].length;
                        }
                        int minLength;
                        if(kResults[i] < topPathDistance)
                            minLength = kResults[i];
                        else
                            minLength = topPathDistance;
                        sim = lcssLength / minLength;*/

                        // Sim 5
                        sim = addLength / kResults[i];
                        addLength = 0;

                        // if (sim > t)
                        // break;
                    }
                    // if (sim <= t)
                    // {
                    // cout << "sim: " << sim << ", topPath:" << topPathID << endl;
                    kResults.push_back(topPathDistance);
                    vvResult.push_back(vvPathCandidateEdge[topPathID]);

                    vkPathNode.push_back(vvPathCandidate[topPathID]);
                    vkPathEdge.push_back(vvPathCandidateEdge[topPathID]);
                    vkPathTime.push_back(vvPathCandidateTime[topPathID]);
                    vResultID.push_back(topPathID);

                    unordered_set<int> pTmp2;
                    for (auto ie = vvPathCandidateEdge[topPathID].begin();
                        ie != vvPathCandidateEdge[topPathID].end(); ie++) {
                        pTmp2.insert(*ie);
                    }
                    pResult.push_back(pTmp2);
                    // }
                }
            }

            vector<int> vTwo;
            vTwo.push_back(topPathID);
            // Find the current top-path class, and its father class
            if (vFather[topPathID] != -1 && !vvmArc[vFather[topPathID]].empty())
                vTwo.push_back(vFather[topPathID]);
            for (auto& pID : vTwo)
            {
                bool bLoop = true;
                while (bLoop)
                {
                    // No More Candidate from current class
                    if (vvmArc[pID].empty())
                    {
                        vvPathCandidate[pID].clear();
                        vvPathCandidateEdge[pID].clear();
                        vvPathCandidateTime[pID].clear();
                        //                    vmPathNodePos[pID].clear();
                        break;
                    }
                    int mineID;
                    int eReducedLen;
                    auto it = vvmArc[pID].begin(); // begin(): edge with minimal deviation cost
    //                mineID = (*it).second; // minimum edge id
    //                countNumber += 1;

                    mineID = (*it).first;
                    eReducedLen = (*it).second;
                    vvmArc[pID].erase(it);

                    // eNodeID1 is also the first point from the deviation edge
                    int eNodeID1 = vEdge[mineID].ID1;
                    int eNodeID2 = vEdge[mineID].ID2;

//                    int posID1 = vmPathNodePos[pID][eNodeID1];
//                    int posID2 = vmPathNodePos[pID][eNodeID2];

                    bool bFixedLoop = false;

                    auto itp = vvmPathNodePos[pID].begin();
                    // eID1Pos is the position of eNodeID1 in vvPathCandidate[pID]
                    int eID1Pos = (*itp).second;
                    vvmPathNodePos[pID].erase(itp);

                    unordered_set<int> sE;
                    //auto& sE = us;
                    // Check for loops
    //                for (int i = vmPathNodePos[pID][eNodeID1]; i < (int) vvPathCandidate[pID].size(); i++)
                    for (int i = eID1Pos; i >= 0; i--)
                    {
                        if (sE.find(vvPathCandidate[pID][i]) == sE.end())
                            sE.insert(vvPathCandidate[pID][i]);
                        else
                        {
                            bFixedLoop = true;
                            break;
                        }
                    }

                    if (bFixedLoop)
                        continue;

                    bLoop = false;
                    vPathDevPrime.push_back(eNodeID2);
                    vPathDeviation.push_back(eNodeID1);
                    //                multimap<int, int> mArcTmp;
                    vector<pair<int, int>> mmArcTmp;
                    vPath.clear();
                    vPathEdge.clear();
                    vPathTime.clear();

                    //                int tpID1 = vvPathCandidateTime[pID][posID1];
                    int tpID1 = vvPathCandidateTime[pID][eID1Pos];
                    int edgeLength = vEdge[mineID].costFunction.getY(tpID1);
                    int tpID2 = tpID1 + edgeLength;
                    struct FPResult TmpSP = getPath(vpOut, tpID2, eNodeID2, ID2); // ��eNodeID2��ʼ
                    if (TmpSP.path.size() == 1) {
                        cout << "No path generated within for loop (in eTDKSPCompare)." << endl;
                        continue;
                    }
                    vPath = TmpSP.path;
                    vPathEdge = TmpSP.pathEdge;
                    vPathTime = TmpSP.vTime;

                    //                mArcTmp = PathDeviationCost(vPath, vPathEdge, vPathTime, vpOut, mineID);
                    //                benchmark::pHeap<3, int, int, int> ArcTmp(2*nodeNum);
                    //                ArcTmp = Arc;
                    PathDeviationCost(Arc, eID1Pos, pID, vPath, vPathEdge, vPathTime, vpOut, mineID);

                    vector<pair<int, int>> mmPosTmp;
                    while (!Arc.empty()) {
                        Arc.extract_min(mmArcEdgeID, mmArcReducedLength, mmArcPosition);
                        mmArcTmp.push_back(make_pair(mmArcEdgeID, mmArcReducedLength));
                        mmPosTmp.push_back(make_pair(mmArcEdgeID, mmArcPosition));
                    }

                    // d(p^(v_i, v_j)) = d(p) + c_ij where c_ij = d(v_j) + w(v_i, v_j) - d(v_i)
    //                int dist = vDistance[pID] + vSPTDistance[eNodeID2] + vEdge[mineID].length - vSPTDistance[eNodeID1] ;
                    int devCost = DeviationCost(tpID1, mineID, vpOut);
                    if (devCost < 0) {
                        cout << "devCost < 0 (in eTDKSPCompare)." << endl;
                        continue;
                    }
                    int dist = vDistance[pID] + devCost;
                    vDistance.push_back(dist);

                    vFather.push_back(pID);
                    qPath.update(vDistance.size() - 1, dist);

                    /*unordered_map<int, int> mE;
                    int i;
                    // Pos stop at eNodeID2 as it is boundary of fixed
                    for (i = 0; i < (int) vPath.size(); i++)
                        mE[vPath[i]] = i;
                    vmPathNodePos.push_back(mE);*/
                    vPath.insert(vPath.begin(), eNodeID1);
                    vPathEdge.insert(vPathEdge.begin(), mineID);
                    vPathTime.insert(vPathTime.begin(), tpID1);

                    vPath.insert(vPath.begin(), vvPathCandidate[pID].begin(), vvPathCandidate[pID].begin() + eID1Pos);
                    vPathEdge.insert(vPathEdge.begin(), vvPathCandidateEdge[pID].begin(), vvPathCandidateEdge[pID].begin() + eID1Pos);
                    vPathTime.insert(vPathTime.begin(), vvPathCandidateTime[pID].begin(), vvPathCandidateTime[pID].begin() + eID1Pos);
                    // Make up the previous sub-path
                    /*for (int j = vmPathNodePos[pID][eNodeID1]; j-1 >= 0; j--)
                    {
    //                    cout << "j:" << vmPathNodePos[pID][eNodeID1];
                        int nodeID = vvPathCandidate[pID][j-1];
                        vPath.insert(vPath.begin(), nodeID);
                        int edgeID = vvPathCandidateEdge[pID][j-1];
                        vPathEdge.insert(vPathEdge.begin(), edgeID);
                        int tp = vvPathCandidateTime[pID][j-1];
                        vPathTime.insert(vPathTime.begin(), tp);
                    }*/
                    //                vmArc.push_back(mArcTmp);
                    vvmArc.push_back(mmArcTmp);
                    vvmPathNodePos.push_back(mmPosTmp);
                    vvPathCandidate.push_back(vPath);
                    vvPathCandidateEdge.push_back(vPathEdge);
                    vvPathCandidateTime.push_back(vPathTime);
                }
            }
            //if (countNumber >= 33836288)
            //{
            //    bCountNumber = false;
            //    break;
            //}
        }
        cout <<" k paths generated." << endl;

        int kr = vkPathEdge.size();
        root->children.resize(kr, nullptr);
        root->numson = kr;
        for (int i = 0; i < kr; ++i) {
            auto& node = root->children[i];
            node = new pathNode();
            node->layer = root->layer + 1;
            //node->fpr.pathEdge = vkPathEdge[i];
            //node->fpr.path = vkPathNode[i];
            node->fpr.pathEdge = vkPathEdge[i];
            node->fpr.path = vkPathNode[i];
            //node->fpr.vTime.swap(vkPathTime[i]);
            node->fpr.vTime = vkPathTime[i];
            int tidx = 0;
            vector<int>& layerTimestamp = gq.layerTimestamp;
            node->rt = root->rt; // initialize
            //while (node->fpr.path[++tidx] != gq.ID2) {
            //    if (node->fpr.vTime[tidx] + node->rt > layerTimestamp[node->layer]) {
            //        node->rt = node->fpr.vTime[tidx] + root->rt;
            //        node->rs = node->fpr.path[tidx];
            //        //nextUpdateTime += tu;
            //        node->fpr.split(tidx + 1);
            //        break;
            //    }

            //    //reQuery(q);
            //    //gq.root->fpr.joint(q.fptmp);

            //}
            bool fs = false;
            while (node->fpr.path[++tidx] != gq.ID2) {
                if (node->fpr.vTime[tidx] > dT + node->fpr.vTime[0]) {
                    // tidx--;
                    node->rt = node->fpr.vTime[tidx];
                    node->rs = node->fpr.path[tidx];
                    // nextUpdateTime += tu;
                    node->fpr.split(tidx + 1);
                    fs = true;
                    break;
                }
                // reQuery(q);
                // gq.root->fpr.joint(q.fptmp);
            }

            if (!fs || node->fpr.path[tidx] == gq.ID2) {
                node->rt = node->fpr.vTime[tidx];
                node->rs = node->fpr.path[tidx];
            }

            if (node->rs != gq.ID2) {
                if (gq.arriveLayer < 0 || node->layer < 2 * gq.arriveLayer)
                    processLayer(gq, node);
            }
            else {
                if (gq.arriveLayer < 0) {
                    gq.arriveLayer = node->layer;
                }
            }
        }
    }

	pmGraph() :Graph() {}
	void restart() {}
	pmGraph(string filename) :Graph(filename) {}

    void selectResult(std::vector<gQuery>& gqs)
    {
        size_t nq = gqs.size();
        std::priority_queue <event, std::vector<event>, less<event> > Ef; // deviation event
        std::vector<event> Eu(nq); // update event
        for (int i = 0; i < nq; ++i) {
            Eu[i].t = gqs[i].t; // departure time
            Eu[i].qid = i; // query id
            gqs[i].curPath = gqs[i].root;
        }
        // Ef.emplace(event())
        int uid = 0;
        // When the query is not traversed, or Ef.size() is not 0
        while (uid < nq || Ef.size()) {
            // if the first query, initialize lastUT, to be consistent with the first departure time
            if (uid == 0) lastUT = Eu[uid].t;
            if (uid < nq) {
                int tu = Eu[uid].t; // at timestamp tu, a query ed.q arrives its deviation node via a sub-path
                if (Ef.size() && Ef.top().t < tu) {
                    event ef = Ef.top();
                    Ef.pop();
                    auto& gq = gqs[ef.qid];
                    processEf(ef, gq);
//                    if (!gq.isReach()){
//                        ef.t = gq.curPath->fpr.vTime[0]; // ef.t is vTime[0] of next sub-path
//                        Ef.emplace(ef);
//                    }
                    //ef.t = gq.nextT;
                    ef.t = gq.curPath->fpr.vTime[0];
                    if (!gq.isReach()) Ef.emplace(ef);
                }
                else {
                    event ef = Eu[uid];
                    auto& gq = gqs[ef.qid];
                    // processEu: determine the ellipse -> grids -> edges
                    // gq: the current new query
                    // Ef: all incomplete queries
                    processEu(Eu[uid], gq, Ef);
                    ef.t = gq.curPath->fpr.vTime[0];
                    if (!gq.isReach()) Ef.emplace(ef);
                    uid++;
                }
            }

            event ef = Ef.top();
            Ef.pop();
            auto& gq = gqs[ef.qid];
            processEf(ef, gq);
//            if (!gq.isReach()){
//                ef.t = gq.curPath->fpr.vTime[0];
//                Ef.emplace(ef);
//            }
            //ef.t = gq.nextT;
            if(gq.curPath!= nullptr){
                ef.t = gq.curPath->fpr.vTime[0];
                if (!gq.isReach()) Ef.emplace(ef);
            }

        }

        for (int i = 0; i < nq; ++i) {
            gqs[i].jointPath();
            int np = gqs[i].finalPath.path.size();
            printf("%d-th query: final path is from node %d to node %d, costing time %d. \n", i, gqs[i].finalPath.path[0], gqs[i].finalPath.path[np - 1], gqs[i].finalPath.getTravelTime());
        }

    }

    int eTDKSPCompare(gQuery& gq)
    {
        if (!gq.root) {
            auto& ID1 = gq.ID1;
            auto& ID2 = gq.ID2;
            auto& DPT = gq.t;
            auto& k = gq.k;
            auto& t = gq.threshold;
            vector<pair<int, LPFunction> > vpOut;

            std::chrono::high_resolution_clock::time_point t1;
            std::chrono::high_resolution_clock::time_point t2;
            std::chrono::duration<double> time_span;

            t1 = std::chrono::high_resolution_clock::now();
            vpOut = BackwardTDFCat(ID2, ID1);
            t2 = std::chrono::high_resolution_clock::now();
            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            cout << "Backward Concatenation Time: " << time_span.count() << "s" << endl;
            allEqual(vpOut);

            gq.root = new pathNode(gq.ID1, gq.t); // pathNode(int s_, int t_) :rs(s_), rt(t_) {}
            gq.root->layer = 1;
            gq.root->fpr = getPath(vpOut, DPT, ID1, ID2);
            //        if(gq.root->fpr.path.size() == 1){
            //            cout << "No initial path generated (in eTDKSPCompare)." << endl;
            //            return -1;
            //        }
            //        return 0;

            auto& initialPath = gq.root->fpr; // fpr: FPResult
            auto& initialSP = initialPath.path;
            // initialSPEdge = initialPath.pathEdge;
            auto& initialSPTime = initialPath.vTime;

            if(initialSP.empty()) {
                cout << "Initial path is empty." << endl;
                return -1;
            }
            // Store the timestamps of speed profile update
            // Based on the global shortest path travel time
            vector<int>& layerTimestamp = gq.layerTimestamp;
            int lts = DPT;
            while (lts < initialSPTime[initialSPTime.size() - 1]) {
                layerTimestamp.push_back(lts);
                lts = lts + dT;
            }

            layerTimestamp.push_back(initialSPTime[initialSPTime.size() - 1]);

            int tidx = 0;
            gq.root->rt = gq.t; // gq.t = departure time
            bool fs = false;
            // dT is \delta T
            while (gq.root->fpr.path[++tidx] != gq.ID2) {
                if (gq.root->fpr.vTime[tidx] > dT + gq.root->fpr.vTime[0]) {
                    // tidx--;
                    // rt: arrival timestamp;
                    // rs: start node;
                    gq.root->rt = gq.root->fpr.vTime[tidx];
                    gq.root->rs = gq.root->fpr.path[tidx];
                    // nextUpdateTime += tu;
                    gq.root->fpr.split(tidx + 1);
                    fs = true;
                    break;
                }
                // reQuery(q);
                // gq.root->fpr.joint(q.fptmp);
            }
            if (!fs || gq.root->fpr.path[tidx] == gq.ID2) {
                gq.root->rt = gq.root->fpr.vTime[tidx];
                gq.root->rs = gq.root->fpr.path[tidx];
            }
        }

        // event eu;
//        processEu(eu);

//        cout << "Running processLayer..." << endl;
        processLayer(gq, gq.root);

        return 0;
    }

    void processEf(event& ef, gQuery& gq) {
        // auto& chosenChain = gq.chosenChain;
        // auto& layer = gq.curLayer;
        // If at the first layer, let us generate the chosenChain.
        if (gq.curPath == gq.root) {
            // chosenChain.clear();
            // gq.curPath = gq.root;
            // chosenChain.push_back(gq.curPath);

            // Check whether the top-1 path exists
            while (!gq.isReach()) { // isReach(): curPath->fpr.path的最后一个node是ID2
                gq.curPath->nextPath = gq.curPath->children[0];
                gq.curPath = gq.curPath->children[0];
                if (!gq.curPath) break;
            }
            if (!gq.isReach()) {
                std::cout << "Error: The top-1 path does not reach the ID2\n";
                exit(-1);
            }
            gq.curPath = gq.root;
        }
        // size_t nt = chosenChain[layer]->fpr.vTime.size();
        // if (lastUT > chosenChain[layer]->fpr.vTime[0] && lastUT <= chosenChain[layer]->fpr.vTime[nt - 1]) { //only when the update time is within the time range of moving at this subpath, we need to recompute the subpath.
        //}

        //else {
        //    layer++;
        //    gq.curPath = gq.curPath->nextPath;

        //}
        //layer++;
//        if(!gq.isReach()){
        gq.curPath = gq.curPath->nextPath;
//        }

    }

    // Process the speed profile update triggered by new query
    void processEu(event& eu, gQuery& gq, priority_queue <event, std::vector<event>, less<event>>& Ef) {
        // if the departure time of new query eu.t exceeds (last update time + update frequency),
        // we need to read a new speed profile, and the sp file id is not necessarily the same as the query id
        if (eu.t > lastUT + uT) { // eu.t: the timestamp when the event occurs
            spid++;
            // Determine the grids (edges) that need to trigger the sp update
            set<int> EfEdges;
            set<int> gqEdges;
            vector<int> commonEdges;

            // Get edges for current new query
            cout << "Get edges for current new query." << endl;
            set<pair<int, int> > sgrids;
            sgrids = ellipseDecompose(gq);
            set<pair<int, int> >::iterator it;
            for (it = sgrids.begin(); it != sgrids.end(); ++it) {
                pair<int, int> coords;
                coords = *it;
                gqEdges.insert(vvGridEdge[coords.first][coords.second].begin(), vvGridEdge[coords.first][coords.second].end());
            }
            // Get the edges for all unfinished queries
            cout << "Get edges for all unfinished queries." << endl;
            priority_queue <event, std::vector<event>, less<event>> Ef2 = Ef;
            EfEdges = trgEdges(Ef2);

            // Get the effected edges
            set<int>::iterator ite;
            for(ite = EfEdges.begin(); ite != EfEdges.end(); ite++){
                if(gqEdges.find(*ite) != gqEdges.end()){
                    commonEdges.push_back(*ite);
                }
            }
            if(commonEdges.size() > 0) {
                cout << "Effected road number: " << commonEdges.size() << endl;


//            cout << "Reading new sp for effected roads..." << endl;

                std::string path = spfile_pre + std::to_string(spid) + spfile_suf;
                readSpeedProfile(path);
//            oldUT = lastUT;
                lastUT = eu.t;
            }
            size_t nt = gq.curPath->fpr.vTime.size();

            // Idea: if the speed profile update is triggered, and the new update time is within the time range of moving at this sub-path,
            // the current sub-path still has to be completed.
            // Meanwhile, all the weights of subsequent candidate paths of unfinished queries need to be re-computed.
            int queuesize = Ef.size();
            for(int i = 0; i < queuesize; i++) {
                event ef = Ef.top();
                Ef.pop();
                gQuery nq;
                nq = Queries[ef.qid];

//                if (lastUT > nq.curPath->fpr.vTime[0] && lastUT <= nq.curPath->fpr.vTime[nt - 1]) {
//                    if (gq.curPath != gq.root) {
                // Update the travel cost of remaining children paths
                updatevTimeforRemainingNodes(nq.curPath);
                nq.findOptPath();
//                    }
//                }
            }

        }
    }

    // Update vTime for remaining sub-paths
    void updatevTimeforRemainingNodes(pathNode* node){
        if (!node) return;
        for(int i = 0; i < node->children.size(); i++){
            vector<int> newvTime;
            pathNode* child = node->children[i];
            int ftsp = child->fpr.vTime[0];
            int curtsp = ftsp;
            newvTime.push_back(ftsp);
            for(int j = 0; j < child->fpr.pathEdge.size(); j++){
                int cost = vEdge[child->fpr.pathEdge[j]].costFunction.getY(curtsp);
                int pretsp = newvTime[newvTime.size()-1];
                newvTime.push_back(pretsp + cost);
                curtsp = pretsp + cost;
            }
            node->children[i]->fpr.vTime = newvTime;

            if(node->numson != 0){
                updatevTimeforRemainingNodes(node->children[i]);
            }
        }

    }

    // Find all edges for unfinished queries
    set<int> trgEdges(priority_queue <event, std::vector<event>, less<event> > Ef){
        set<int> AllEdges;
        int base = pow(2, power);
        double xBase = vpXYBase.first;
        double yBase = vpXYBase.second;

        int totalupdateRoadNum = 0;
        int queuesize = Ef.size();

        set<pair<int, int> > sGrids;
        set<int> edgeIDs;
        for(int i = 0; i < queuesize; i++){
            event ef = Ef.top();
            Ef.pop();
            // Update the information of origin node and construct a new query as the input of triggerSPUpdate function
            gQuery nq;
            nq = Queries[ef.qid];
            nq.ID1 = Queries[ef.qid].curPath->rs; // get current ID1
            nq.ed = Eucli(vNode[nq.ID1].lng, vNode[nq.ID1].lat, vNode[nq.ID2].lng, vNode[nq.ID2].lat);

            nq.x1 = int((vNode[nq.ID1].lng - minLong) / xBase); // grid coordinates
            nq.y1 = int((vNode[nq.ID2].lat - minLat) / yBase);

            if(nq.x1 == base)
                nq.x1 = base-1;
            if(nq.y1 == base)
                nq.y1 = base-1;
            
            sGrids = ellipseDecompose(nq);
            set<pair<int, int> >::iterator it;
            for (it = sGrids.begin(); it != sGrids.end(); ++it) {
                pair<int, int> coords;
                coords = *it;
                AllEdges.insert(vvGridEdge[coords.first][coords.second].begin(), vvGridEdge[coords.first][coords.second].end());
            }
        }
//        cout << "Find all edges for unfinished queries." << endl;
        return AllEdges;
    }
};



//int Graph::eTDKSPCompare(int ID1, int ID2, int DPT, int k, vector<int>& kResults, vector<vector<int>>& vkPathNode,
//    vector<vector<int>>& vkPathEdge, vector<vector<int>>& vkPathTime, double t, int& countNumber, int& popPath, float& SimTime);

