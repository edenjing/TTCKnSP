#include "graph.h"

// Determine the next visiting node (parent node)
int Graph::getvP(int vx, LPFunction& lpf){
    int pos;
    if(lpf.vX.empty()){
        cout << "lpf.vX is empty (in getvP)." << endl;
        return -1;
    }
    if(vx > lpf.vX[lpf.vX.size()-1]){
        cout << "vx > lpf.vX[lpf.vX.size()-1] (in getvP)." << endl;
        return -1;
    }
    if(vx > upperbound){
        cout << "vx > upperbound (in getvP)." << endl;
        return -1;
    }
    if(vx == lpf.vX[lpf.vX.size()-1]){
        return lpf.vP[lpf.vP.size()-1];
    }

    pos = getPos(lpf.vX, vx);

    int res = lpf.vP[pos];
    //if (res < 0) {
    //    std::cout << "res<0, res= " << res << std::endl;
    //    std::cout << "res<0, res= " << res << ", pos= " << pos << std::endl;
    //    exit(-1);
    //}
    return res;
}

int Graph::getPos(vector<int>& vX, int vx) {
    if (vx == vX[0]) {
        return 0;
    } 
    else {
        for (int i = 1; i <= vX.size() - 1; i++) {
            if (vX[i - 1] <= vx && vx < vX[i]) {
//                cout << i << "," << vX[i-1] << endl;
                return i - 1;
            }
        }

        return vX.size() - 1;// ??? why vs do not report the fatal error: undefined behavior-no returned value
    }
}

struct FPResult Graph::getPath(vector<pair<int, LPFunction>>& vpOut, int depTime, int ID1, int ID2)
{
    struct FPResult result;

    vector<int> vPath;
    vector<int> vPathEdge;
    vector<int> vTime;

    vPath.push_back(ID1);
    vPathEdge.push_back(-1);
    vTime.push_back(depTime);

    result.path = vPath;
    result.pathEdge = vPathEdge;
    result.vTime = vTime;

    int middleNode; // middle node of the path
    int middleTimestamp;
    middleNode = ID1;
    middleTimestamp = depTime;

    int nextnode;
    int edgelength;
    int nextTimestamp;
    if(ID1 == ID2){
        cout << "No path from current node to ID2." << endl;
        return result;
    }
    while(middleNode != ID2){
        nextnode = getvP(middleTimestamp, vpOut[middleNode].second);
        //std::cout << "Nextnode: " << nextnode << std::endl;
        // no next node returned
        if(nextnode == -1){
            cout << "No next node returned (in getPath)." << endl;
            vPath.push_back(-1);
            break;
        }
        vPath.push_back(nextnode);
        for(int i = 0; i < adjListEdge[middleNode].size(); i++)
        {
            if(adjListEdge[middleNode][i].first == nextnode)
            {
                vPathEdge.push_back(adjListEdge[middleNode][i].second);
                edgelength = vEdge[adjListEdge[middleNode][i].second].costFunction.getY(middleTimestamp);
                nextTimestamp = edgelength + middleTimestamp;
                vTime.push_back(nextTimestamp);
                break;
            }
        }
        middleNode = nextnode;
        middleTimestamp = nextTimestamp;
    }

    if(vPath[vPath.size()-1] != ID2){
        cout << "No edge connected to ID2 (in getPath)." << endl;
//        vPath.push_back(ID2);
        return result;
    }
    if(vPathEdge.size()!=1){
        vPathEdge.erase(vPathEdge.begin());
    }
    result.path = vPath;
    result.pathEdge = vPathEdge;
    result.vTime = vTime;

    printf("GetPath: Start from Node %d at time %d to Node %d at time %d, travel time is: %d.\n", ID1, vTime[0], ID2, vTime[vTime.size() - 1], vTime[vTime.size() - 1] - vTime[0]);

    return result;
}

int Graph::DeviationCost(int depTime, int eID, vector<pair<int, LPFunction> >& vpOut)
{
    int reducedCost;
    int edgeCost; // travel time of deviation edge with depTime
    edgeCost = vEdge[eID].costFunction.getY(depTime);
    int tdtID1 = vpOut[vEdge[eID].ID1].second.getY(depTime);
    int tdtID2 = vpOut[vEdge[eID].ID2].second.getY(depTime + edgeCost);
    // Remember to check if there will be negative numbers
    reducedCost = tdtID2 + edgeCost - tdtID1;
//    if(reducedCost < 0){
//        cout << "reducedCost < 0." << endl;
//        return -1;
//    }else{
//        return reducedCost;
//    }
    return reducedCost;
}

// Get the deviation costs for all edges in current path
void Graph::PathDeviationCost(benchmark::pHeap<3, int, int, int>& Arc, int eID1Pos, int pID,
                              vector<int> sp, vector<int> spEdge, vector<int> spTime,
                              vector<pair<int, LPFunction> >& vpOut, int mineID){
//    benchmark::pHeap<3, int, int, int> Arc(2*nodeNum);

    int deviationCost;
    unordered_set<int> sE;
    for(int i = 0; i < sp.size()-1; i++) // 遍历到ID2的上一个点
    {
        int p = sp[i];  // if pID != -1, sp starts with eNodeID2
        sE.insert(p);
        int e = spEdge[i];
        int dpt = spTime[i];
//        int count = 1;
        for (int j = 0; j < (int)adjListEdge[p].size(); ++j)
        {
            int eID = adjListEdge[p][j].second;
            int nodeID2 = vEdge[eID].ID2;
            if(sE.find(nodeID2) != sE.end()) // loop exists
                continue;
            // Check if eID2 is connected to the destination node
            if(eID != e && eID != mineID && !vpOut[nodeID2].second.vX.empty()){
                deviationCost = DeviationCost(dpt, eID, vpOut);
                if(deviationCost < 0){
                    continue;
                }else{
                    // mArc.insert(make_pair(deviationCost, eID));
                    if(pID == -1){ // Update the Arc of first shortest path
                        Arc.update(eID, deviationCost, i);
                    }else{
                        Arc.update(eID, deviationCost, i + 1 + eID1Pos);
                    }
                }
            }
        }
//        count++;
    }
//    return Arc;
}

bool Graph::allEqual(vector<pair<int, LPFunction> >& vpOut){
    int count = 0;
    for(int i = 0; i < vpOut.size(); i++){
        // 判断vX, vY, vP的元素个数是否相等
        if(vpOut[i].second.vP.size() != vpOut[i].second.vX.size()){
            cout << "Node ID:\t" << i << endl;
            cout << "vP.size():\t" << vpOut[i].second.vP.size() << endl;
            cout << "vX.size():\t" << vpOut[i].second.vX.size() << endl;
        }
//            bool exist = false;
        // Here should exist nodes that return false.

        if(vpOut[i].second.vP.size() == 0){
//                cout << "vpOut[i].second.vP.size() == 0" << endl;
            continue;
        }else{
            bool exist = std::equal(vpOut[i].second.vP.begin()+1, vpOut[i].second.vP.end(), vpOut[i].second.vP.begin());
            if(exist){
//                    cout << "elements in vpOut[i].second.vP are equal." << endl;
                continue;
            }else{
                count ++;
//                    cout << "elements in vpOut[i].second.vP are not equal." <<endl;
            }
        }
    }
    cout << "Number of elements that are not equal in vPOut: " << count << endl;
//    return 0;
}

// Get the backward minimal time-dependent functions from all vertices to destination
// vP in lpf: parent node
// sourceID: destination node
vector<pair<int, struct LPFunction>> Graph::BackwardTDFCat(int sourceID, int ID1)
{
//    cout << "Pruned Backward Search " << sourceID << endl;
    boost::heap::fibonacci_heap<compareNode, boost::heap::compare<compareNode>> fHeap;
    vector<boost::heap::fibonacci_heap<compareNode, boost::heap::compare<compareNode>>::handle_type> vHandler(nodeNum);

    vector<int> vTime(nodeNum, INF);
    vector<bool> vbHeap(nodeNum, false); // Store if in Heap
    vector<bool> bVisited(nodeNum, false);

    vector<int>::iterator ivNN, ivNR;
    int topNodeID, neighborNodeID, neighborRoadID;
    int i, minCost;
    bool bUpdated;

    vTime[sourceID] = 0;
    compareNode cn;
    cn.pif = make_pair(sourceID, 0); // ID, minCost
    vHandler[sourceID] = fHeap.push(cn);

    vbHeap[sourceID] = true;
    bVisited[sourceID] = true;
    bool bQ;
    vector<bool> bSkipped(nodeNum, false);

    vector<pair<int, LPFunction>> vpOut;
    // 判读是否被拼接过
    vector<bool> vExist(nodeNum, false);
    for(i = 0; i < nodeNum; i++){
        LPFunction lpf;
        vpOut.push_back(make_pair(sourceID, lpf));
    }

    compareNode cnTop;

    vector<bool> vb(nodeNum, false);
    int remain = nodeNum;
    int boundMinCost = INF;
    while(!fHeap.empty())
    {
//        int topCost;
        cnTop = fHeap.top();
        fHeap.pop();

        topNodeID = cnTop.pif.first;
//        vpOut[topNodeID].second.display();
        vbHeap[topNodeID] = false;
        bVisited[topNodeID] = true;

//		cout << "topNodeID:" <<  topNodeID << endl;

        if(bSkipped[topNodeID])
            continue;

        if(!vb[topNodeID])
            remain--;
        vb[topNodeID] = true;

//        Stop the concatenation early
//        if(topNodeID == ID1){
//            boundMinCost = vpOut[ID1].second.getMin() * 1.5;
//        }
//
//        if(vpOut[topNodeID].second.getMin() > boundMinCost)
//            break;

//        cout << sourceID << " Backward Visiting " << topNodeID << endl;
//        cout << "Visiting: " << topNodeID << endl;
//        cout << "Remaining:" << remain << endl << endl;

        if(vpOut[topNodeID].second.getSize() == 1)
            continue;

        for(i = 0; i < (int)adjListEdgeR[topNodeID].size(); i++)
        {
//			cout << "i:" << i << "\t" << adjListEdgeR[topNodeID].size() << endl;

            bUpdated = false;
            bQ = false;

            neighborNodeID = adjListR[topNodeID][i];
            neighborRoadID = adjListEdgeR[topNodeID][i].second;

            if(bSkipped[neighborNodeID])
                if(neighborNodeID == sourceID)
                    continue;
            minCost = vTime[neighborNodeID];

//        Stop the concatenation early
//            if(minCost > boundMinCost)
//                break;

            bool bLPF = false;
            LPFunction lpf;
            if(topNodeID == sourceID) // sourceID = root
            {
//                cout << "neighborNodeID: " << neighborNodeID << endl;
                // 初始化的vp存的是sourceID
                lpf = vEdge[neighborRoadID].costFunction;

                if(sourceID == vEdge[neighborRoadID].ID1)
                {
                    lpf.setID1(vEdge[neighborRoadID].ID2);
                    lpf.setID2(vEdge[neighborRoadID].ID1);
                }
//                lpf.setArrival(false);
            }
            else
            {
//                cout << "topNodeID:" << topNodeID << endl;
//                cout << "neighborNodeID: " << neighborNodeID << endl;
//                cout << "LPFCat Operation: " << endl;

                LPFunction lpftmp = vEdge[neighborRoadID].costFunction;
                // 本身就是反向遍历edge，所以其实并不需要交换ID1和ID2
                if(topNodeID == vEdge[neighborRoadID].ID1)
                {
                    lpftmp.setID1(vEdge[neighborRoadID].ID2);
                    lpftmp.setID2(vEdge[neighborRoadID].ID1);
                }

//                cout << "lpftmp:" << endl;
//                lpftmp.display();

//                cout << "vpOut:" << endl;
//                vpOut[topNodeID].second.display();
                lpf = lpftmp.LPFCat(vpOut[topNodeID].second); // 这里拼接完的lpf全部以lpftmp的ID2为vp

                if (lpf.vX.empty()){
                    lpf.minY = INF;
                    lpf.maxY = -1;
//                    cout << "vpOut[topNodeID].second.vX is empty (before LPFCat)." << endl;
                    bLPF = true;
                    //continue;
                }
                else if(lpf.vX[lpf.vX.size()-1] > upperbound){
                    lpf.minY = INF;
                    lpf.maxY = -1;
//                    cout << "lpf.vX[lpf.vX.size()-1] > upperbound (before LPFCat)." << endl;
//					cout << "Jump out" << endl;
//                    cout << "vX size:" << lpf.vX.size() << endl;
//                    lpf.display();
//                    cout << "i:" << i << endl;
                    bLPF = true;
                    //continue;
                }
            }
//            cout << "OK?" << endl;

//            cout << "lpf:" << endl;
//            cout << lpf.vX.size() << endl;
//            lpf.display();
//            cout << "neighbor vpOut:" << endl;
//            vpOut[neighborNodeID].second.display();

            if(bLPF){
//                cout  << "continue" << endl;
                continue;
            }

            //if(lpf.vX.empty() || lpf.vX[lpf.vX.size()-1] > upperbound){
            //  continue;
            //}
            // 测试lpf是否dominate: LPFCat之后得到lpf，如果vpOut内现有函数完全在lpf之下，则不再比大小
            // if(!vpOut[neighborNodeID].second.vX.empty())
            if(!bLPF)
            {
                if(vpOut[neighborNodeID].second.dominate(lpf)){
//                cout << "Skip! " << neighborNodeID << endl;
                    bSkipped[neighborNodeID] = true;
                    continue;
                }
            }

//			lpf.display();
//			cout << "neighbor:" << neighborNodeID << endl;

            if(!vExist[neighborNodeID]){
                vpOut[neighborNodeID].second = lpf;
                vExist[neighborNodeID] = true;
                bUpdated = true;
            }
            else{
                LPFunction lpfmin;
                lpfmin = vpOut[neighborNodeID].second.LPFMinNew3(lpf);
//                if(lpfmin.vX.empty() || lpfmin.vX[lpfmin.vX.size()-1] > upperbound){
////                    cout << "Max x value in lpf exceeds Upperbound: " << lpf.vX[lpf.vX.size()-1] << endl;
//                    cout << "Max x value in lpfmin exceeds Upperbound or vX is empty (after LPFMinNew3)."<< endl;
//                }

                vpOut[neighborNodeID].second = lpfmin;
                bUpdated = true;
            }

            minCost = vpOut[neighborNodeID].second.getMin();

            // Updated and not in Heap
            if((!vbHeap[neighborNodeID] && bUpdated) || !bVisited[neighborNodeID])
            {
                // vTime存储每个neighborNode的minimum cost
                vTime[neighborNodeID] = minCost;
                compareNode cn;
                cn.pif = make_pair(neighborNodeID, minCost);
                vHandler[neighborNodeID] = fHeap.push(cn);
                vbHeap[neighborNodeID] = true;
                bVisited[neighborNodeID] = true;
            }
                // Updated and in Heap, key changed
            else if(bUpdated && vbHeap[neighborNodeID])
            {
                vTime[neighborNodeID] = minCost;
                (*vHandler[neighborNodeID]).pif.second = vTime[neighborNodeID];
                fHeap.decrease(vHandler[neighborNodeID]);
            }
//            cout << "So???" << endl;
        }
//        cout << "So?" << endl;
    }
//    cout << endl;

    int count = 0;
    for(int i = 0; i < vpOut.size(); i++){
        if (vpOut[i].second.getSize() != 0){
//            cout << "Node Pair: (" << vpOut[i].second.ID1 << "," << vpOut[i].second.ID2 << ")" << endl;
            count++;
        }
    }
    cout << "Number of concatenated vertices: " << count << endl;

    return vpOut;
}

vector<int> Graph::LCSS(vector<int> &p1, vector<int> &p2)
{
    int len1 = p1.size();
    int len2 = p2.size();
    vector<vector<int>> result(len1+1,vector<int>(len2+1));
    vector<vector<int>> result2(len1+1, vector<int>(len2+1));
    for(int i = 0;i <= len1; i++)
        for(int j = 0; j <= len2; j++)
        {
            if(i == 0||j == 0) result[i][j] = 0;
            else if(p1[i-1] == p2[j-1]){
                result[i][j] = result[i-1][j-1]+1;
                result2[i][j] = p1[i-1];
            }
            else result[i][j] = max(result[i][j-1], result[i-1][j]);
        }
//    return result[len1][len2];

    vector<int> lcss(edgeNum);
    int a = len1;
    int b = len2;
    for(int i = result[len1][len2]; i > 0; i--){
        bool flag = false;
        for(int j = a; j >= 0; j--){
            for(int k = b; k >= 0; k--){
                if(result[j][k] == i && result2[j][k] != 0){
                    lcss.push_back(result2[j][k]);
                    a = j;
                    b = k;
                    flag = true;
                    break;
                }
            }
            if(flag) break;
        }
    }
    reverse(lcss.begin(), lcss.end());
    return lcss;
}

int Graph::readSpeedProfile(string filename)
{
    ifstream ifSP(filename);
    if(!ifSP){
        cout << "Cannot open Speed Profile " << filename << endl;
        return -1;
    }
    cout << "Reading " << filename << endl;

    double cost;
    int num, i, t;
    int roadID;
    int minY = INF;
    int maxY = -1;

    while(ifSP >> roadID)
    {
        vector<int> vX, vY, vP;
        ifSP >> num;
        // For Beijing SP Data
//        if(roadID == 204)
//        {
//            cout << "num= " << num << endl;
//            cout << endl;
//        }
//        if(num != 288){
//            vEdge[roadID].costFunction = vEdge[roadID-1].costFunction;
//            continue;
//        }

        vX.assign(num, 0);
        vY.assign(num, 0);

        for(i = 0; i < num; i++){
            ifSP >> t >> cost;
            if(t >= num)
                break;
            // Test data:
//            vX[i] = t*300;
            // Porto data:
            vX[i] = t*900;
            vY[i] = cost;
            if(cost < minY)
                minY = cost;
            if(cost > maxY)
                maxY = cost;
        }
//		LPFunction f = LPFunction(vX, vY, g.vRoad[roadID].ID1, g.vRoad[roadID].ID2, t, maxY, minY);
        vP.assign(num, vEdge[roadID].ID2);
        if (vX.empty()){
            cout << "Edge: " << roadID << " vX is empty!" << endl;
        }
        LPFunction f = LPFunction(vX, vY, vP, vEdge[roadID].ID1, vEdge[roadID].ID2, upperbound, maxY, minY);
        vEdge[roadID].costFunction = f;
    }

    ifSP.close();

    for(int i = 0; i < (int)vEdge.size(); i++)
        vEdge[i].costFunction.calMM();
    cout << "Speed profile loading finish." << endl;
    return 0;
}

// Get the coordinates of all nodes
// Pay attention to the order of longitude and latitude
int Graph::readMap(string filename)
{
    ifstream inGraph(filename);
    if(!inGraph)
        cout << "Cannot open Map " << filename << endl;
    cout << "Reading " << filename << endl;

    string line;
    getline(inGraph,line);
    vector<string> vs;
    boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);

    // Porto Data
    // longitude: -8.69369 -8.53874
    // latitude: 41.1382 41.1822
    minLong = stod(vs[1]);
    maxLong = stod(vs[2]);
    minLat = stod(vs[3]);
    maxLat = stod(vs[4]);

    int ID1;

    getline(inGraph,line);
    while(!inGraph.eof())
    {
        vector<string> vs;
        boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
        ID1 = stoi(vs[0]);

        struct Node n;
        n.nodeID = ID1;
        n.lng = stod(vs[2]);
        n.lat = stod(vs[3]);

        vNode.push_back(n);
        getline(inGraph,line);
    }
    inGraph.close();
    cout << "minLat: " << minLat << "\t" << "maxLat: " << maxLat << "\t" << "minLong: " << minLong << "\t" << "maxLong: " << maxLong << endl;
    return nodeNum;
}

int Graph::readRoad(string filename)
{
    ifstream inGraph(filename);
    if(!inGraph)
        cout << "Cannot open Roadfile " << filename << endl;
    cout << "Reading " << filename << endl;

    string line;

    getline(inGraph,line);
    if(line[0] == '%')
    {
        vector<string> vs;
        boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
        nodeNum = stoi(vs[1]);
        edgeNum = stoi(vs[2]);
        cout << "nodeNum: " << nodeNum << "\t" << "edgeNum: " << edgeNum << endl;
    }

    int ID1, ID2, edgeID;
    adjList.resize(nodeNum);
    adjListR.resize(nodeNum);
    adjListEdge.resize(nodeNum);
    adjListEdgeR.resize(nodeNum);
    int edgeCount = 0;

//    getline(inGraph,line);
//
    while(!inGraph.eof())
    {
        getline(inGraph,line);
        if (line.size() == 0) break;
        vector<string> vs;
        boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
        edgeID = stoi(vs[0]);
        ID1 = stoi(vs[1]);
        ID2 = stoi(vs[2]);

        struct Edge e;
        e.ID1 = ID1;
        e.ID2 = ID2;
        e.edgeID = edgeID;

        bool bExisit = false;
        for(int i = 0; i < (int)adjList[ID1].size(); i++)
        {
            if(adjList[ID1][i] == ID2)
            {
                bExisit = true;
                break;
            }
        }

        if(!bExisit)
        {
            vEdge.push_back(e);
            adjList[ID1].push_back(ID2);
            adjListR[ID2].push_back(ID1);
            adjListEdge[ID1].push_back(make_pair(ID2, edgeID)); // edgeCount实际表示edgeId
            adjListEdgeR[ID2].push_back(make_pair(ID1, edgeID));
            edgeCount++;
        }
    }

//    vbISO.assign(nodeNum, false); // vbISO的作用是啥?
    inGraph.close();
    return nodeNum;
}

int Graph::readQuery(string filename){
    int base = pow(2, power);
    ifstream inGraph(filename);
    Queries.reserve(2000000);
    vpXYBase = make_pair((double)(maxLong - minLong) / base, (double)(maxLat - minLat) / base);
    double xBase = vpXYBase.first;
    double yBase = vpXYBase.second;

    set<int> sTmp;
    vector<set<int> > vsTmp;
    vsTmp.assign(base, sTmp);
    vvsGridF.assign(base, vsTmp);
    vvsGridB.assign(base, vsTmp);

    if(!inGraph)
        cout << "Cannot open query file:" << filename << endl;
    cout << "Reading " << filename << endl;
//    int pID1, pID2, dpt;
//    vector<int> ID1List;
//    vector<int> ID2List;
//    vector<int> DPTList;
    string line;
    getline(inGraph, line);
    while(!inGraph.eof())
    {
        gQuery q;
        vector<string> vs;
        boost::split(vs, line, boost::is_any_of(" "), boost::token_compress_on);
        q.qID = Queries.size();
        q.ClusterID = -1;

        q.ID1 = stoi(vs[0]);
        q.ID2 = stoi(vs[1]);
        q.t = stoi(vs[2]);
        q.direction = Angle(vNode[q.ID1], vNode[q.ID2]);
        q.ed = Eucli(vNode[q.ID1].lng, vNode[q.ID1].lat, vNode[q.ID2].lng, vNode[q.ID2].lat);

        q.x1 = int((vNode[q.ID1].lng - minLong) / xBase); // grid coordinate
        q.y1 = int((vNode[q.ID2].lat - minLat) / yBase);

        if(q.x1 == base)
            q.x1 = base-1;
        if(q.y1 == base)
            q.y1 = base-1;
        vvsGridF[q.x1][q.y1].insert(q.qID);

        q.x2 = int((vNode[q.ID2].lng - minLong) / xBase);
        q.y2 = int((vNode[q.ID2].lat - minLat) / yBase);
        if(q.x2 == base)
            q.x2 = base-1;
        if(q.y2 == base)
            q.y2 = base-1;
        vvsGridB[q.x2][q.y2].insert(q.qID);

        Queries.push_back(q);
        getline(inGraph, line);
    }
    inGraph.close();
    cout << "Queries loading finish." << endl;
    return 0;
}

int Graph::Eucli(double lng1, double lat1, double lng2, double lat2)
{
    int lat=(int)(abs(lat1-lat2)*111319);
    int lon=(int)(abs(lng1-lng2)*83907);
    int min,max;
    min=(lat>lon)?lon:lat;
    max=(lat>lon)?lat:lon;
    int approx=max*1007+min*441;

    if(max<(min<<4))
        approx-=max*40;
    return (approx+512)>>10;
}

// d2 is the travel cost of actual optimal path
double Graph::shortestPathError(int &d1, int &d2){
    return (double)abs(d1-d2)/d2;
}

struct FPResult Graph::SSFP(int ID1, int ID2, int t){
//    std::pair<vector<int>, vector<int>> result;
    struct FPResult result;
    vector<int> path;
    vector<int> pathEdge;
    vector<int> pathTime;

    if (ID1 == ID2) {
        cout << "ID1 = ID2." << endl;
        return result;
    }

    if (t > upperbound) {
        cout << "Departure time exceeds time domain upperbound." << endl;
        return result;
    }

    benchmark::heap<2, int, int> queue(nodeNum);
    queue.update(ID1, t);
    vector<int> vTime(nodeNum, INF);
    vector<bool> vbVisited(nodeNum, false);
    vector<int> Parents(nodeNum, -1);
//    vector<int>::iterator ivD, ivP, ivNL;
    int i;
    int topNodeID, neighborNodeID, neighborRoadID;

    vTime[ID1] = t; // store the timestamp
    int currentTime;

    bool find = false;
    while (!queue.empty()) {
        int topTime;
        queue.extract_min(topNodeID, topTime);
        vbVisited[topNodeID] = true;
        if (topNodeID == ID2) {
            find = true;
            break;
        }
        currentTime = t + topTime;
        for (i = 0; i < adjList[topNodeID].size(); i++) {
            neighborNodeID = adjList[topNodeID][i];
            neighborRoadID = adjListEdge[topNodeID][i].second;

            int cost = vEdge[neighborRoadID].costFunction.getY(currentTime) + topTime;
            if (!vbVisited[neighborNodeID]) {
                if (vTime[neighborNodeID] > cost) {
                    Parents[neighborNodeID] = topNodeID;
                    vTime[neighborNodeID] = cost;
                    queue.update(neighborNodeID, cost);
                }
            }
        }

    }

    // store the fastest path
    if (!find) {
        cout << "there is no path from " << ID1 << "to " << ID2 << "." << endl;
        return result;
    }

    int tmpnode;
    int curNode = ID2;
//    path.emplace_back(ID2);
    while (curNode != ID1) {
        path.emplace_back(curNode);
        pathTime.emplace_back(vTime[curNode]);

        tmpnode = Parents[curNode];
        if(tmpnode != -1) {
            for(int i = 0; i < adjListEdge[tmpnode].size(); i++) {
                if(adjListEdge[tmpnode][i].first == curNode)
                    pathEdge.push_back(adjListEdge[tmpnode][i].second);
            }
        } else
            break;
        curNode = tmpnode;
    }
    path.emplace_back(ID1);
    pathTime.emplace_back(t);

    vector<int> p;
    vector<int> pedge;
    vector<int> ptime;

    for (int i = path.size() - 1; i >= 0; i--) {
        p.emplace_back(path[i]);
    }
    for(int i = pathEdge.size() - 1; i >= 0; i--) {
        pedge.emplace_back(pathEdge[i]);
    }
    for(int i = pathTime.size() -1; i >= 0; i--) {
        ptime.emplace_back(pathTime[i]);
    }

//    result = std::make_pair(p, vTime);
    result.path = p;
    result.pathEdge = pedge;
    result.vTime = ptime;
    return result;
}

