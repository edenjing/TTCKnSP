#include "graph.h"
#include "heap.h"

void Graph::initCoherence(){

    int base = pow(2, power);
//    vBase.push_back(base);
    pair<double, int> pTmp;
    vector<pair<double, int> > vTmp;
    vTmp.assign(base, pTmp);
    vvCoherence.assign(base, vTmp);

    set<int> eTmp;
    vector<set<int>> eeTmp;
    eeTmp.assign(base, eTmp);
    vvGridEdge.assign(base, eeTmp);
}

// construct vvCoherence
void Graph::createCoherence(){

    int base = pow(2, power);
    double xBase = vpXYBase.first;
    double yBase = vpXYBase.second;

    // Collect nodes in each grid
    vector<vector<vector<int> > > vvGridNode;
    vector<int> vTmp;
    vector<vector<int> > vvTmp;
    vvTmp.assign(base, vTmp);
    vvGridNode.assign(base, vvTmp);

    int x, y;
    for(int i = 0; i < vNode.size(); i++) {
        x = (vNode[i].lng - minLong) / xBase;
        y = int((vNode[i].lat - minLat) / yBase);
        if(x == base)
            x = base-1;
        if(y == base)
            y = base-1;
        vvGridNode[x][y].push_back(i); // store the node ids
    }

// Try to collect edges in each grid
//    int x1, y1;
//    int x2, y2;
//    for(int i = 0; i < vEdge.size(); i++) {
//
//        x1 = (vNode[vEdge[i].ID1].lng - minLong) / xBase;
//        y1 = int((vNode[vEdge[i].ID1].lat - minLat) / yBase);
//        if(x1 == base)
//            x1 = base-1;
//        if(y1 == base)
//            y1 = base-1;
//
//        x2 = (vNode[vEdge[i].ID2].lng - minLong) / xBase;
//        y2 = int((vNode[vEdge[i].ID2].lat - minLat) / yBase);
//        if(x2 == base)
//            x2 = base-1;
//        if(y2 == base)
//            y2 = base-1;
//
//        // store the edge ids
//        if(x1 == x2 && y1 == y2) vvGridEdge[x1][y1].insert(i);
//        // classification is not finished
//    }

    double a2 = PI/2;
    double a3 = PI;
    double a4 = PI*3/2;
    double a5 = 2*PI;

    // Roads are inferred by node's neighbors
    for(int i = 0; i < base; i++) {
        for (int j = 0; j < base; j++) {
            if (vvGridNode[i][j].size() == 0) {
                // vector<vector<pair<double, int>>> vvCoherence
                vvCoherence[i][j] = make_pair(0,0);
                continue;
            }

            // road length sum
            int roadNum = 0;
            double angleSum = 0;
            // For each road, compute direction
            for(int k = 0; k < (int)vvGridNode[i][j].size(); k++)
            {
                int nodeID = vvGridNode[i][j][k];
                for(int r = 0; r < (int)adjList[nodeID].size(); r++)
                {
//					roadNum += g->Edges[nodeID][r].second;
                    roadNum++;
//                    int neighbor = g->Edges[nodeID][r].first;
                    int neighbor = adjList[nodeID][r];
                    // Construct vvGridEdge
                    // if the neighbor node also exists in the grid, determine the edge id
                    vector<int>::iterator it;
                    it = find(vvGridNode[i][j].begin(), vvGridNode[i][j].end(), neighbor);
                    if(it != vvGridNode[i][j].end()){
                        int roadId;
                        for(int l = 0; l < (int)adjListEdge[nodeID].size(); l++){
                            if (adjListEdge[nodeID][l].first == neighbor){
                                roadId = adjListEdge[nodeID][l].second;
                            }
                        }
                        vvGridEdge[i][j].insert(roadId);
                    }

                    double angle = Angle(vNode[nodeID], vNode[neighbor]);
                    double ad = angle;
                    double ad2 = abs(angle-a2);
                    double ad3 = abs(angle-a3);
                    double ad4 = abs(angle-a4);
                    double ad5 = abs(angle-a5);

                    if(ad2 < ad)
                        ad = ad2;
                    if(ad3 < ad)
                        ad = ad3;
                    if(ad4 < ad)
                        ad = ad4;
                    if(ad5 < ad)
                        ad = ad5;

                    if(ad < 0.001) // ?
                        ad = 0;
                    //angleSum += ad * g->Edges[nodeID][r].second;
                    angleSum += ad;

//					cout << ad << "\t" << angle << endl;
                }
            }
            vvCoherence[i][j] = make_pair(angleSum/roadNum, roadNum);
        }
    }
    cout << "Create Coherence Finished." << endl;
}

void Graph::coveredGrids(double x1, double y1, double x2, double y2, double constDist, set<pair<int, int> >& sGrids)
{
    int base = pow(2, power);
    double xBase = vpXYBase.first;
    double yBase = vpXYBase.second;

    //<level, <x,y>>
//	vector<pair<int, int> > vGrids;
    queue<pair<int, int > > gridQ;

    // <lon, lat>, in/not ellipse
    // For fast check
    map<pair<double, double>, bool> mpB;
    map<pair<double, double>, bool>::iterator impB;
    int pLevel = -2;
//    for(int i = 0; i < level; i++)
//    {
//    double xBase = vpXYBase.first;
//    double yBase = vpXYBase.second;
    for(int j = 0; j < base; j++)
    {
        for(int k = 0; k < base; k++)
        {
            if(vvCoherence[j][k].second == 0)
                continue;

            // x - lng; y - lat
            pair<double, double> SW = make_pair(minLong+ j*xBase, minLat + k*yBase);
            pair<double, double> SE = make_pair(minLong + (j+1)*xBase, minLat + k*yBase);
            pair<double, double> NW = make_pair(minLong + j*xBase, minLat + (k+1)*yBase);
            pair<double, double> NE = make_pair(minLong + (j+1)*xBase, minLat + (k+1)*yBase);
            vector<pair<double, double> > vpP;
            vector<pair<double, double> >::iterator ivpP;
            vpP.push_back(SW);
            vpP.push_back(SE);
            vpP.push_back(NW);
            vpP.push_back(NE);

            int pCount = 0;
            for(ivpP = vpP.begin(); ivpP != vpP.end(); ivpP++)
            {
                impB = mpB.find(*ivpP);
                if(impB != mpB.end())
                {
                    if((*impB).second)
                        pCount++;
                }
                else
                {
                    int dp = Eucli((*ivpP).first, (*ivpP).second, x1, y1) + Eucli((*ivpP).first, (*ivpP).second, x2, y2);
                    if(dp <= constDist)
                    {
                        mpB[*ivpP] = true;
                        pCount++;
                    }
                    else
                        mpB[*ivpP] = false;
                }
            }

            // fully covered, add all grids
            if(pCount == 4) {
                addAllChildren(j, k, sGrids);
            }
            else if(pCount > 0) // Partially covered, push to queue
            {
                //				if(gridQ.empty())
                //					pLevel = i;
                gridQ.push(make_pair(j,k));
            }
        }
    }

//    if(!gridQ.empty())
//        break;
//    }
//	cout << "Queue size:" << gridQ.size() << endl;
    pair<int, int > currentGrid;
    while(!gridQ.empty())
    {
        currentGrid = gridQ.front();
        gridQ.pop();
//         int currentLevel = currentGrid.first; // level
        int gridX = currentGrid.first;
        int gridY = currentGrid.second;
//			addAllChildren(currentLevel+1, 2*gridX, 2*gridY, sGrids);
//        double xBase = vpXYBase[currentLevel+1].first;
//        double yBase = vpXYBase[currentLevel+1].second;

        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                // cross the border
                if((gridX*2 + j) >= base || (gridY*2 + k) >= base){
                    continue;
                }
                // second is roadNum
                if(vvCoherence[gridX*2 + j][gridY*2 + k].second == 0)
                    continue;

                pair<double, double> SW = make_pair(minLong + (gridX*2 + j)*xBase, minLat + (gridY*2 + k)*yBase);
                pair<double, double> SE = make_pair(minLong + (gridX*2 + (j+1))*xBase, minLat + (gridY*2 + k)*yBase);
                pair<double, double> NW = make_pair(minLong + (gridX*2 + (j+1))*xBase, minLat + (gridY*2 + (k+1))*yBase);
                pair<double, double> NE = make_pair(minLong + (gridX*2 + j)*xBase, minLat + (gridY*2 + (k+1))*yBase);

                vector<pair<double, double> > vpP;
                vector<pair<double, double> >::iterator ivpP;
                vpP.push_back(SW);
                vpP.push_back(SE);
                vpP.push_back(NW);
                vpP.push_back(NE);

                int pCount = 0;
                for(ivpP = vpP.begin(); ivpP != vpP.end(); ivpP++)
                {
                    impB = mpB.find(*ivpP);
                    if(impB != mpB.end())
                    {
                        if((*impB).second)
                            pCount++;
                    }
                    else
                    {
                        int dp = Eucli((*ivpP).first, (*ivpP).second, x1, y1) + Eucli((*ivpP).first, (*ivpP).second, x2, y2);
                        if(dp <= constDist)
                        {
                            mpB[*ivpP] = true;
                            pCount++;
                        }
                        else
                            mpB[*ivpP] = false;
                    }

                    //fully covered, add all grids
                    if(pCount > 0)
                    {
                        sGrids.insert(make_pair(2*gridX+j, 2*gridY+k));
                        //					addAllChildren(currentLevel+1, 2*gridX+j, 2*gridY+k, sGrids);
                    }
                }
            }
        }
//        }
    }
}

// No level concept in our application
void Graph::addAllChildren(int x, int y, set<pair<int, int> >& sGrids)
{
//    int ldiff = level - currentLevel;
//    int b1 = pow(2, ldiff);
//    int b2 = pow(2, ldiff-1);
//    for(int i = x; i < x + 1; i++)
//    {
//        for(int j = y; j < y + 1; j++)
//        {
//            if(vvCoherence[i][j].second != 0)
//                sGrids.insert(make_pair(i,j));
//        }
//    }
    if(vvCoherence[x][y].second != 0) // second is road number
        sGrids.insert(make_pair(x,y));
}

// The origin node of input query gq may not be consistent with original one
// Because the ellipse is determined during the routing process
// Output: edge ids covered in the ellipse
set<pair<int, int> > Graph::ellipseDecompose(gQuery& gq){
    double xBase = vpXYBase.first;
    double yBase = vpXYBase.second;

    double a2 = PI/2;
    double a3 = PI;
    double a4 = PI*3/2;
    double a5 = 2*PI;
    int qSize = 0;
    double theta1 = PI / 12;
    double theta2 = (23 / 12) * PI;

    double x1, x2, y1, y2; // road network coordinates
    x1 = vNode[gq.ID1].lng;
    y1 = vNode[gq.ID1].lat;
    x2 = vNode[gq.ID2].lng;
    y2 = vNode[gq.ID2].lat;

//    if(x1 == x2 || y1 == y2)
//        continue;

    double k = (double)(y2-y1)/(x2-x1);
    double qAngle = gq.direction;
    double ad = qAngle;
    double ad2 = abs(qAngle-a2);
    double ad3 = abs(qAngle-a3);
    double ad4 = abs(qAngle-a4);
    double ad5 = abs(qAngle-a5);

    if(ad2 < ad)
        ad = ad2;
    if(ad3 < ad)
        ad = ad3;
    if(ad4 < ad)
        ad = ad4;
    if(ad5 < ad)
        ad = ad5;

    if(ad < 0.001)
        ad = 0;

    int xDiff = abs(gq.x1-gq.x2); // grid coordinates
    int yDiff = abs(gq.y1-gq.y2);


    double angleSum = 0;
    int roadNum = 0;
    int gCount = 0;
    vector<pair<int ,int> > vTraverseGrid;
    if(xDiff >= yDiff)
    {
        if(x1 <= x2)
        {
            for(double i = x1; i <= x2;)
            {
                double y = y1 + (i-x1)*k;
                i += xBase;
                int xtmp = int((i-minLong) / xBase);
                int ytmp = int((y-minLat) / yBase);
                vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                roadNum += vvCoherence[xtmp][ytmp].second;
                gCount++;
            }
        }
        else
        {
            for(double i = x1; i >= x2;)
            {
                double y = y1 + (i-x1)*k;
                i -= xBase;
                int xtmp = int((i-minLong) / xBase);
                int ytmp = int((y-minLat) / yBase);
                vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                roadNum += vvCoherence[xtmp][ytmp].second;
                gCount++;
            }
        }
    }
    else
    {
        if(y1 <= y2)
        {
            for(double i = y1; i <= y2;)
            {
                double x = (i-y1) / k + x1;
                i += yBase;
                int xtmp = int((x-minLong) / xBase);
                int ytmp = int((i-minLat) / yBase);
                vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                roadNum += vvCoherence[xtmp][ytmp].second;
                gCount++;
            }
        }
        else
        {
            for(double i = y1; i >= y2;)
            {
                double x = (i-y1) / k + x1;
                i -= yBase;
                int xtmp = int((x-minLong) / xBase);
                int ytmp = int((i-minLat) / yBase);
                vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                roadNum += vvCoherence[xtmp][ytmp].second;
                gCount++;
            }
        }
    }

    double coherenceDirection = angleSum / roadNum;
    double qAngleDiff = abs(ad - coherenceDirection);

    double cosQA = cos(qAngleDiff);
    double constDist = 2 * gq.ed / (1 + cosQA);
//		cout << "Const Dist:" <<  constDist << endl;

    double fx, fy;
    double dfs = 2 * gq.ed * cosQA / (1 + cosQA);
    fx = x1 + dfs * cos(qAngle) / 83907;
    fy = y1 + dfs * sin(qAngle) / 111319;

    set<pair<int, int> > sGrids;
    set<pair<int, int> >::iterator  isGrids;

    // Output: sGrids
    coveredGrids(x1, y1, fx, fy, constDist, sGrids);
    for(int i = 0; i < (int)vTraverseGrid.size(); i++)
        sGrids.insert(vTraverseGrid[i]);

//    cout << "Ellipse decomposition finished." << endl;

    int x, y;
    pair<int, int> coords;
    set<pair<int, int>>::iterator it;
    int totalRoadnum = 0; // total number of roads that need to be updated
    // set<pair<int, int> > sGrids;
    for (auto it = sGrids.begin(); it != sGrids.end(); it++) {
        coords = *it;
        x = coords.first;
        y = coords.second;
        totalRoadnum += vvCoherence[x][y].second; // roadNum
    }
    cout << "Number of roads in ellipse: " << totalRoadnum << endl;
    return sGrids;

    // Display Ellipse
/*		vector<vector<int> > vTest;
    vector<int> v(gc.vBase[level], 0);
    vTest.assign(gc.vBase[level], v);
    for(isGrids = sGrids.begin(); isGrids != sGrids.end(); isGrids++)
        vTest[(*isGrids).first][(*isGrids).second] = 1;

    for(int i = 0; i < gc.vBase[level]; i++)
    {
        for(int j = 0; j < gc.vBase[level]; j++)
        {
            cout << vTest[i][j] << " ";
        }
        cout << endl;
    }
*/
//    vvsGridF[gq.x1][gq.y1].erase(gq.qID);
//    vvsGridB[gq.x2][gq.y2].erase(gq.qID);

//    ellCluster c;
//    c.clusterID = vCluster.size();
//    c.direction = qAngle;
//    c.d = q.ed;
//    set<int>::iterator isQF;
//    c.sGrid = sGrids;
//    c.sQuery.insert(q.qID);
////        c.sQuerySorted.insert(make_pair(q.ed, q.qID));
//    for(isGrids = sGrids.begin(); isGrids != sGrids.end(); isGrids++)
//    {
//        set<int>& qSetF = vvsGridF[(*isGrids).first][(*isGrids).second];
//        for(isQF = qSetF.begin(); isQF != qSetF.end();)
//        {
////                if(!vbQuery[*isQF])
////                {
////                    isQF++;
////                    continue;
////                }
//            gQuery& q2 = Queries[*isQF];
//            if(q2.ClusterID != -1)
//            {
//                isQF++;
//                continue;
//            }
//
//            if(sGrids.find(make_pair(q2.x2, q2.y2)) != sGrids.end())
//            {
//                double qd2 = abs(q2.direction - q.direction);
//                if(qd2 <= theta1 || qd2 >= theta2)
//                {
//                    q2.ClusterID = c.clusterID;
//                    vvsGridB[q2.x2][q2.y2].erase(*isQF);
//                    c.sQuery.insert(*isQF);
//                    c.sQuerySorted.insert(make_pair(Queries[*isQF].ed, *isQF));
//                    isQF = qSetF.erase(isQF);
//                }
//                else
//                    isQF++;
//            }
//            else
//                isQF++;
//        }
//
//    }
//    qSize += c.sQuery.size();
////		cout << "Cluster:" << c.clusterID << "\t" << c.d <<  "\t" << c.sQuery.size() << "\t" << qSize << endl;
//    vCluster.push_back(c);
//    }

}

// Output: vCluster (which contains sGrid): the covered grids in each ellipse (i.e., each query)
void Graph::RegionDecomposition(){
    double xBase = vpXYBase.first;
    double yBase = vpXYBase.second;

    double a2 = PI/2;
    double a3 = PI;
    double a4 = PI*3/2;
    double a5 = 2*PI;
    int qSize = 0;
    double theta1 = PI / 12;
    double theta2 = (23 / 12) * PI;

    for(int qIndex = 0; qIndex < (int)Queries.size(); qIndex++)
    {

        gQuery& q = Queries[qIndex];


        double x1, x2, y1, y2;
        x1 = vNode[q.ID1].lng;
        y1 = vNode[q.ID1].lat;
        x2 = vNode[q.ID2].lng;
        y2 = vNode[q.ID2].lat;

        if(x1 == x2 || y1 == y2)
            continue;

        double k = (double)(y2-y1)/(x2-x1);
        double qAngle = q.direction;
        double ad = qAngle;
        double ad2 = abs(qAngle-a2);
        double ad3 = abs(qAngle-a3);
        double ad4 = abs(qAngle-a4);
        double ad5 = abs(qAngle-a5);

        if(ad2 < ad)
            ad = ad2;
        if(ad3 < ad)
            ad = ad3;
        if(ad4 < ad)
            ad = ad4;
        if(ad5 < ad)
            ad = ad5;

        if(ad < 0.001)
            ad = 0;

        int xDiff = abs(q.x1-q.x2);
        int yDiff = abs(q.y1-q.y2);


        double angleSum = 0;
        int roadNum = 0;
        int gCount = 0;
        vector<pair<int ,int> > vTraverseGrid;
        if(xDiff >= yDiff)
        {
            if(x1 <= x2)
            {
                for(double i = x1; i <= x2;)
                {
                    double y = y1 + (i-x1)*k;
                    i += xBase;
                    int xtmp = int((i-minLong) / xBase);
                    int ytmp = int((y-minLat) / yBase);
                    vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                    angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                    roadNum += vvCoherence[xtmp][ytmp].second;
                    gCount++;
                }
            }
            else
            {
                for(double i = x1; i >= x2;)
                {
                    double y = y1 + (i-x1)*k;
                    i -= xBase;
                    int xtmp = int((i-minLong) / xBase);
                    int ytmp = int((y-minLat) / yBase);
                    vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                    angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                    roadNum += vvCoherence[xtmp][ytmp].second;
                    gCount++;
                }
            }
        }
        else
        {
            if(y1 <= y2)
            {
                for(double i = y1; i <= y2;)
                {
                    double x = (i-y1) / k + x1;
                    i += yBase;
                    int xtmp = int((x-minLong) / xBase);
                    int ytmp = int((i-minLat) / yBase);
                    vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                    angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                    roadNum += vvCoherence[xtmp][ytmp].second;
                    gCount++;
                }
            }
            else
            {
                for(double i = y1; i >= y2;)
                {
                    double x = (i-y1) / k + x1;
                    i -= yBase;
                    int xtmp = int((x-minLong) / xBase);
                    int ytmp = int((i-minLat) / yBase);
                    vTraverseGrid.push_back(make_pair(xtmp, ytmp));
                    angleSum += vvCoherence[xtmp][ytmp].first * vvCoherence[xtmp][ytmp].second;
                    roadNum += vvCoherence[xtmp][ytmp].second;
                    gCount++;
                }
            }
        }

        double coherenceDirection = angleSum / roadNum;
        double qAngleDiff = abs(ad - coherenceDirection);

        double cosQA = cos(qAngleDiff);
        double constDist = 2 * q.ed / (1 + cosQA);
//		cout << "Const Dist:" <<  constDist << endl;

        double fx, fy;
        double dfs = 2 * q.ed * cosQA / (1 + cosQA);
        fx = x1 + dfs * cos(qAngle)/83907;
        fy = y1 + dfs * sin(qAngle)/111319;

        set<pair<int, int> > sGrids;
        set<pair<int, int> >::iterator  isGrids;

        // Output: sGrids
        coveredGrids(x1, y1, fx, fy, constDist, sGrids);
        for(int i = 0; i < (int)vTraverseGrid.size(); i++)
            sGrids.insert(vTraverseGrid[i]);

        // Display Ellipse
/*		vector<vector<int> > vTest;
		vector<int> v(gc.vBase[level], 0);
		vTest.assign(gc.vBase[level], v);
		for(isGrids = sGrids.begin(); isGrids != sGrids.end(); isGrids++)
			vTest[(*isGrids).first][(*isGrids).second] = 1;

		for(int i = 0; i < gc.vBase[level]; i++)
		{
			for(int j = 0; j < gc.vBase[level]; j++)
			{
				cout << vTest[i][j] << " ";
			}
			cout << endl;
		}
*/
        vvsGridF[q.x1][q.y1].erase(q.qID);
        vvsGridB[q.x2][q.y2].erase(q.qID);

        ellCluster c;
        c.clusterID = vCluster.size();
        c.direction = qAngle;
        c.d = q.ed;
        set<int>::iterator isQF;
        c.sGrid = sGrids;
        c.sQuery.insert(q.qID);
//        c.sQuerySorted.insert(make_pair(q.ed, q.qID));
        for(isGrids = sGrids.begin(); isGrids != sGrids.end(); isGrids++)
        {
            set<int>& qSetF = vvsGridF[(*isGrids).first][(*isGrids).second];
            for(isQF = qSetF.begin(); isQF != qSetF.end();)
            {
//                if(!vbQuery[*isQF])
//                {
//                    isQF++;
//                    continue;
//                }
                gQuery& q2 = Queries[*isQF];
                if(q2.ClusterID != -1)
                {
                    isQF++;
                    continue;
                }

                if(sGrids.find(make_pair(q2.x2, q2.y2)) != sGrids.end())
                {
                    double qd2 = abs(q2.direction - q.direction);
                    if(qd2 <= theta1 || qd2 >= theta2)
                    {
                        q2.ClusterID = c.clusterID;
                        vvsGridB[q2.x2][q2.y2].erase(*isQF);
                        c.sQuery.insert(*isQF);
                        c.sQuerySorted.insert(make_pair(Queries[*isQF].ed, *isQF));
                        isQF = qSetF.erase(isQF);
                    }
                    else
                        isQF++;
                }
                else
                    isQF++;
            }

        }
        qSize += c.sQuery.size();
//		cout << "Cluster:" << c.clusterID << "\t" << c.d <<  "\t" << c.sQuery.size() << "\t" << qSize << endl;
        vCluster.push_back(c);
    }

    cout << "Total Query:" << qSize << endl;

//    t2 = std::chrono::high_resolution_clock::now();
//    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
//    cout << "Coherene Decomposition takes " << time_span.count() << endl;

    cout << "Region Decomposition Finished." << endl;
}

void Graph::showCluster()
{
//    ofstream outCluster(filename.c_str());
    int rCount =0;
    int qCount =0;
    int cCount =0;
    int sCount =0;
    set<int>::iterator isQ;
    for(int i = 0; i < vCluster.size(); i++)
    {
        qCount += vCluster[i].sQuery.size();
        if(vCluster[i].sQuery.size() > 0)
        {
//            outCluster << cCount << "\t" << vCluster[i].sQuery.size() << endl;
            cout << cCount << "\t" << vCluster[i].sQuery.size() << endl;

            for(isQ = vCluster[i].sQuery.begin(); isQ != vCluster[i].sQuery.end(); isQ++)
            {
                int qID = *isQ;
                gQuery q = Queries[qID];
//                outCluster << q.ID1 <<"\t" << q.ID2 << "\t" << q.h <<"\t" << q.m <<"\t" << q.td <<"\t" << q.d <<endl;
                cout << q.ID1 <<"\t" << q.ID2 << "\t" << q.ed << endl;
                rCount++;
            }
            cCount++;
        }
        else
        {
            sCount++; // the number of cluster that contain less than xxx queries
        }
    }
    cout << "Total query size: " << qCount << "\t" << "Total ellipse size: " << cCount << endl;
//    cout << "Total query size: " << sCount << "\t" << "Total ellipse size: " << rCount << endl;
}

