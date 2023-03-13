#include "linearPiecewiseFunction.h"
#include "tools.h"

int LPFunction::getID1() const
{
    return ID1;
}

int LPFunction::getID2() const
{
    return ID2;
}

int	LPFunction::getSize()	const
{
    return vX.size();
}

int LPFunction::getUBound() const
{
    return upperBound;
}

int LPFunction::getMaxX()   const
{
    return vX[vX.size()-1];
}

void LPFunction::setID1(int id1)
{
    ID1 = id1;
}

void LPFunction::setID2(int id2)
{
    ID2 = id2;
}

void LPFunction::setUBound(int uBound)
{
    upperBound = uBound;
}

void LPFunction::setvP(vector<int> VP)
{
    vP = VP;
}

int LPFunction::calMax()
{
    vector<int>::iterator iv;
    int MaxTmp = -1;
    for(iv = this->vY.begin(); iv != this->vY.end(); iv++)
        if(*iv > MaxTmp)
            MaxTmp = *iv;

    this->maxY = MaxTmp;
    return MaxTmp;
}

int LPFunction::calMin()
{
    vector<int>::iterator iv;
    int MinTmp = INF;
    for(iv = this->vY.begin(); iv != this->vY.end(); iv++)
        if(*iv < MinTmp)
            MinTmp = *iv;

    this->minY = MinTmp;
    return MinTmp;
}

void LPFunction::calMM()
{
    vector<int>::iterator iv;
    int MinTmp = INF;
    int MaxTmp = -1;

    for(iv = this->vY.begin(); iv != this->vY.end(); iv++)
    {
        if(*iv < MinTmp)
            MinTmp = *iv;
        if(*iv > MaxTmp)
            MaxTmp = *iv;
    }

    this->minY = MinTmp;
    this->maxY = MaxTmp;
}

bool LPFunction::redundant(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if(x3 <= x1 || x3 <= x2)
        return true;

    int y = (int)(y3 - (y3-y1)*(x3-x2)/(x3-x1));
    if(y == y2  && y2 != minY)
    {
        return true;
    }
    else
        return false;
}

vector<int>& LPFunction::returnVX()
{
    return vX;
}

vector<int>& LPFunction::returnVY()
{
    return vY;
}

int LPFunction::setValue(vector<int>& vX, vector<int>& vY)
{
    minY = INF;
    maxY = -1;
    vector<int>::iterator ivX, ivY;
    this->vX.clear();
    this->vY.clear();

    this->vX.reserve(vX.size());
    this->vY.reserve(vY.size());
    int i = 0;

    for(ivX = vX.begin(), ivY = vY.begin(); ivY != vY.end(); ivX++, ivY++)
    {
        if(ivX != vX.begin() && *ivX <= *(ivX-1))
            continue;

        if(ivX - vX.begin() < 2)
        {
            this->vX.push_back(*ivX);
            this->vY.push_back(*ivY);

            if(*ivY < minY)
                minY = *ivY;
            if(*ivY > maxY)
                maxY = *ivY;
        }
        else
        {
            if(redundant(*(this->vX.end()-2), *(this->vY.end()-2), *(this->vX.end()-1), *(this->vY.end()-1), *ivX, *ivY) && i == 0)
            {
                this->vX.erase(this->vX.end()-1);
                this->vY.erase(this->vY.end()-1);
                i = 1;
            }

            if(*ivX > *(this->vX.end()-1))
            {
                this->vX.push_back(*ivX);
                this->vY.push_back(*ivY);

                if(i > 0)
                    i++;

                if(i > 4)
                    i = 0;

                if(*ivY < minY)
                    minY = *ivY;
                if(*ivY > maxY)
                    maxY = *ivY;
            }
        }
    }

    while(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] > upperBound)
    {
        this->vX.erase(this->vX.end()-1);
        this->vY.erase(this->vY.end()-1);
    }

    if(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] < upperBound)
    {
        this->vX.push_back(upperBound - *(this->vY.end()-1));
        this->vY.push_back(*(this->vY.end()-1));
    }

    return this->vX.size();
}

// New version of setValue function
int LPFunction::setValue2(vector<int>& vX, vector<int>& vY, vector<int>& vP)
{
    //  cout << "???" << endl;
    minY = INF;
    maxY = -1;
    vector<int>::iterator ivX, ivY, ivP;
    //  cout << "???" << endl;
    this->vX.clear();
    this->vY.clear();
    this->vP.clear();
    //  cout << "???" << endl;
    this->vX.reserve(vX.size());
    this->vY.reserve(vY.size());
    this->vP.reserve(vP.size());
    int i = 0;
    //cout << "???" << endl;
    if(vX.size()==0)
    {
        cout << "vX.size()==0 in setValue2()!" << endl;
    }
    //  cout << "In setValue2" << endl;
    for(ivX = vX.begin(), ivY = vY.begin(), ivP = vP.begin(); ivY != vY.end(); ivX++, ivY++, ivP++)
    {
        if(ivX != vX.begin() && *ivX <= *(ivX-1))
            continue;
        if(*ivX + *ivY > upperBound || *ivX > upperBound)
            break;

        //cout << "ivX:" << *ivX  << "\tthis ivX:" << *(this->vX.end()-1) << "\tthis vX size:" << this->vX.size() << endl;
        if(ivX - vX.begin() < 2)
        {
            this->vX.push_back(*ivX);
            this->vY.push_back(*ivY);
            this->vP.push_back(*ivP);

            if(*ivY < minY)
                minY = *ivY;
            if(*ivY > maxY)
                maxY = *ivY;
        }
        else
        {
            if(redundant(*(this->vX.end()-2), *(this->vY.end()-2), *(this->vX.end()-1), *(this->vY.end()-1), *ivX, *ivY) && i == 0)
            {
                this->vX.erase(this->vX.end()-1);
                this->vY.erase(this->vY.end()-1);

                this->vP.erase(this->vP.end()-1);

//                i = 1;
            }

            if(*ivX > *(this->vX.end()-1))
            {
                this->vX.push_back(*ivX);
                this->vY.push_back(*ivY);

                this->vP.push_back(*ivP);

                /*              if(i > 0)
                                  i++;

                              if(i > 4)
                                  i = 0;
              */
                if(*ivY < minY)
                    minY = *ivY;
                if(*ivY > maxY)
                    maxY = *ivY;
            }
        }
    }

//    cout << "fr VX size:" << this->vX.size() << endl;
//    cout << "upperbound:" << this->upperBound << endl;

    if(this->vX.size() < 2)
        return this->vX.size();
    //while(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] > upperBound)
    while(*(this->vX.end()-1) > upperBound)
    {
        this->vX.erase(this->vX.end()-1);
        this->vY.erase(this->vY.end()-1);

        this->vP.erase(this->vP.end()-1);
    }

    if(*(this->vX.end()-1) < upperBound)
        //if(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] < upperBound)
    {
//        this->vX.push_back(upperBound - *(this->vY.end()-1));
        this->vX.push_back(upperBound);
        this->vY.push_back(*(this->vY.end()-1));

        this->vP.push_back(*(this->vP.end()-1));
    }

    return this->vX.size();
}
//int LPFunction::setValue(vector<int>& vX, vector<int>& vY)
//{
//    minY = INF;
//    maxY = -1;
//    vector<int>::iterator ivX, ivY;
//    this->vX.clear();
//    this->vY.clear();
//
//    this->vX.reserve(vX.size());
//    this->vY.reserve(vY.size());
//    int i = 0;
//
//    for(ivX = vX.begin(), ivY = vY.begin(); ivY != vY.end(); ivX++, ivY++)
//    {
//        if(ivX != vX.begin() && *ivX <= *(ivX-1))
//            continue;
//
//        if(ivX - vX.begin() < 2)
//        {
//            this->vX.push_back(*ivX);
//            this->vY.push_back(*ivY);
//
//            if(*ivY < minY)
//                minY = *ivY;
//            if(*ivY > maxY)
//                maxY = *ivY;
//        }
//        else
//        {
//            if(redundant(*(this->vX.end()-2), *(this->vY.end()-2), *(this->vX.end()-1), *(this->vY.end()-1), *ivX, *ivY) && i == 0)
//            {
//                this->vX.erase(this->vX.end()-1);
//                this->vY.erase(this->vY.end()-1);
//                i = 1;
//            }
//
//            if(*ivX > *(this->vX.end()-1))
//            {
//                this->vX.push_back(*ivX);
//                this->vY.push_back(*ivY);
//
//                if(i > 0)
//                    i++;
//
//                if(i > 4)
//                    i = 0;
//
//                if(*ivY < minY)
//                    minY = *ivY;
//                if(*ivY > maxY)
//                    maxY = *ivY;
//            }
//        }
//    }
//
//    if(!this->isArrival())
//    {
//        while(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] > upperBound)
//        {
//            this->vX.erase(this->vX.end()-1);
//            this->vY.erase(this->vY.end()-1);
//        }
//
//        if(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] < upperBound)
//        {
//            this->vX.push_back(upperBound - *(this->vY.end()-1));
//            this->vY.push_back(*(this->vY.end()-1));
//        }
//    }
//    else
//    {
//        while(*(this->vX.end()-1) > upperBound)
//        {
//            this->vX.erase(this->vX.end()-1);
//            this->vY.erase(this->vY.end()-1);
//        }
//        if(*(this->vX.end()-1) < upperBound)
//        {
//            this->vX.push_back(upperBound);
//            this->vY.push_back(*(this->vY.end()-1));
//        }
//    }
//
//    return this->vX.size();
//}

pair<int, int>	LPFunction::getTimeDomain() const
{
    if(vX.empty())
        return make_pair(-1,-1);

    return make_pair(vX[0], vX[vX.size()-1]);
}

//original
int	LPFunction::getY(int x) const
{
    if(vX.empty())
        throw runtime_error("The function is Empty!");

    if(x < vX[0])
    {
//		cout << "x:" << x << "\tvX[0]:" << vX[0] << endl;
        return vY[0];
//		throw runtime_error("Input is smaller than the lower bound");
    }

    if(x > vX[vX.size()-1])
    {
//		cout << "Input:" << x << "\t" << vX[vX.size()-1] << endl;
        return vY[vY.size()-1];
//		throw runtime_error("Input is larger than the upper bound");
    }

    vector<int>::const_iterator low, up;
    low = lower_bound(vX.begin(), vX.end(), x);
    if (low != vX.begin()) low--;
    up = low + 1;

    int pos = low - vX.begin();

    if(*low == x)
        return vY[pos];

    int x1 = *low;
    int x2 = *up;
    int y1 = vY[pos];
    int y2 = vY[pos+1];

    if(x1 == x2)
        return y1;
    else
        return computeY(x1, x2, y1, y2, x);

}

vector<int>	LPFunction::getVY(vector<int> inX)
{
    int x1, y1, x2, y2, x, y;
    vector<int> vrY;
    vector<int>::iterator ivinX, ivX, ivY;
    vrY.reserve(inX.size());

//	if(inX.empty())
//		return vrY;

    ivinX = inX.begin();
    while(*ivinX < *(vX.begin()) && ivinX < inX.end())
    {
        vrY.push_back(*(vY.begin()));
        ivinX++;
    }

    for(ivX = vX.begin(), ivY = vY.begin(); ivX + 1 != vX.end();)
    {
        if(ivinX >= inX.end())
            break;
        if(*ivX<=*ivinX && *(ivX+1)>=*ivinX)
        {
            x1 = *ivX;
            y1 = *ivY;
            x2 = *(ivX+1);
            y2 = *(ivY+1);
            x = *ivinX;
            y = computeY(x1, x2, y1, y2, x);
            vrY.push_back(y);

            ivinX++;
        }
        else if(*ivX > *ivinX)
        {
            ivinX++;
        }
        else if(*(ivX+1) < *ivinX)
        {
            ivX++;
            ivY++;
        }
    }

    int minX = *(vX.begin());
    int minY = *(vY.begin());
    int maxX = *(vX.end() - 1);
    int maxY = *(vY.end() - 1);
    while(ivinX < inX.end())
    {
        vrY.push_back(maxY);
        ivinX++;
    }

    return vrY;
}

vector<int> LPFunction::getVYHistogram(int n)
{
    int step = 86400 / (n * 60);
    int x = 0;
    vector<int> vrY(n, 0);
    vector<int>::iterator ivX, ivY;
    ivX = vX.begin();
    ivY = vY.begin();
    int x1, y1, x2, y2, y;

    //min,max for lpf's x value
    int minX = *(vX.begin());
    int minY = *(vY.begin());
    int maxX = *(vX.end() - 1);
    int maxY = *(vY.end() - 1);

    int i = 0;
    while(x < minX)
    {
        vrY[i] = minY;
        x += step;
        i++;
        cout << x << "\t" << minX << endl;
    }

    for(ivX = vX.begin(), ivY = vY.begin(); ivX + 1 != vX.end() && i < n;)
    {
        if(x > maxX)
        {
            vrY[i] = maxY;
            x += step;
            i++;
            if(i >= n)
                break;
            continue;
        }

        if(*ivX <= x && *(ivX+1) >= x)
        {
            x1 = *ivX;
            y1 = *ivY;
            x2 = *(ivX+1);
            y2 = *(ivY+1);
            y = computeY(x1, x2, y1, y2, x);
            vrY[i] = y;
        }
        else if(*ivX > x)
        {
            i++;
            x += step;
        }
        else if(*(ivX+1) < x)
        {
            ivX++;
            ivY++;
        }
    }

    while(i < n)
    {
        vrY[i] = maxY;
        i++;
    }

    return vrY;
}

int LPFunction::computeY(int x1, int x2, int y1, int y2, int x) const
{
    if(x1 == x2)
        return y1;
    return Round((y2-y1)*(x - x1)/(x2-x1) + y1);
}

bool LPFunction::equal(LPFunction f2)
{
    vector<int>::iterator ivX1, ivX2, ivY1, ivY2;
    for(ivX1 = vX.begin(), ivX2 = f2.vX.begin(), ivY1 = vY.begin(), ivY2 = f2.vY.begin(); ivX1 != vX.end(); ivX1++, ivX2++, ivY1++, ivY2++)
    {
        if(*ivX1 - *ivX2 > 2 || *ivX2 - *ivX1 > 2)
            return false;
        else if(*ivY1 - *ivY2 > 2 || *ivY2 - *ivY1 > 2)
            return false;
    }

    return true;
}

//LPFunction LPFunction::LPFCatArrival(LPFunction f2, bool bNI, int uBound)
//{
//    vector<int>::iterator ivX, ivY, ivY2;
//    vector<int> vrX, vrY, vf2X, vf2Y;
//    int x, y, f2y;
//    int xMin = *(vX.begin());
//    int xMax = *(vX.end()-1);
//    map<int, int> mX;
//    map<int, int>::iterator imX;
//
//    for(ivX = vX.begin(); ivX != vX.end(); ivX++)
//        mX[*ivX] = 0;
//
//    for(ivX = f2.vX.begin(); ivX != f2.vX.end(); ivX++)
//    {
//        if(*ivX > xMax)
//            break;
//        if(*ivX >= xMin)
//            mX[*ivX] = 0;
//    }
//
//    for(imX = mX.begin(); imX != mX.end(); imX++)
//    {
//        if((*imX).first > upperBound)
//            continue;
//        f2y = f2.getY((*imX).first);
//        x = (*imX).first + f2y;
//        y = f2y + getY((*imX).first);
//        vrX.push_back(x);
//        vrY.push_back(y);
//    }
//
//    if(!vrX.empty())
//        if(vrX[vrX.size()-1] < x)
//        {
//            vrX.push_back(x);
//            vrY.push_back(y);
//        }
//
//    LPFunction fr;
//    fr.setID1(ID1);
//    if(f2.ID2 == ID2)
//        fr.setID2(f2.ID1);
//    else
//        fr.setID2(f2.ID2);
//    fr.setUBound(uBound);
//    fr.setArrival(true);
//    fr.setValid(true);
//    fr.setValue(vrX, vrY);
//
//	cout << "In Cat:" << endl;
//	fr.display();
//
//    if(bNI)
//    {
//        fr.LPFNon_Inc(uBound);
//		cout << "Non:" << endl;
//		fr.display();
//    }
//
//	fr.display();
//    return fr;
//}
//
//LPFunction	LPFunction::LPFCatArrivalPrune(LPFunction f2, bool bNI, int uBound, int min, int max)
//{
//    vector<int>::iterator ivX, ivY, ivY2;
//    vector<int> vrX, vrY, vf2X, vf2Y;
//    int x, y, f2y;
//    int xMin = *(vX.begin());
//    int xMax = *(vX.end()-1);
//    map<int, int> mX;
//    map<int, int>::iterator imX;
//
//    for(ivX = vX.begin(); ivX != vX.end(); ivX++)
//        mX[*ivX] = 0;
//
//    for(ivX = f2.vX.begin(); ivX != f2.vX.end(); ivX++)
//    {
//        if(*ivX > xMax)
//            break;
//        if(*ivX >= xMin)
//            mX[*ivX] = 0;
//    }
//
//    int rMin = INF;
//    for(imX = mX.begin(); imX != mX.end(); imX++)
//    {
//        if((*imX).first > upperBound)
//            continue;
//        f2y = f2.getY((*imX).first);
//        x = (*imX).first + f2y;
//        y = f2y + getY((*imX).first);
//        vrX.push_back(x);
//        vrY.push_back(y);
//        if(y < rMin)
//            rMin = y;
//    }
//
//    if(!vrX.empty())
//        if(vrX[vrX.size()-1] < x)
//        {
//            vrX.push_back(x);
//            vrY.push_back(y);
//        }
//
//    int x1, x2, x3, y1, y2, y3;
//    int ytmp;
//
//	for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
//		cout << "(" << *ivX << "," << *ivY << ")";
//	cout << endl;
//
//    int yUP = rMin + max;
//    if(vrX.size() > 3)
//    {
//        for(ivX = vrX.begin()+1, ivY = vrY.begin()+1; ivX != vrX.end()-2;)
//        {
//		cout << "(" << *ivX << "," << *ivY << ")";
//            x1 = *(ivX-1);
//            y1 = *(ivY-1);
//            x2 = *ivX;
//            y2 = *ivY;
//            x3 = *(ivX+1);
//            y3 = *(ivY+1);
//            ytmp = computeY(x1, x3, y1, y3, x2);
//
//			if((ytmp + min > yUP && y2 >= ytmp) || (y2 + min > yUP && y2 <= ytmp))
//			if(ytmp + min > yUP && y2 >= ytmp)
//            if(ytmp + min > max && y2 >= ytmp)
//            {
//				cout << "Prune " << *ivX  << "\t" << *ivY << endl;
//                vrX.erase(ivX);
//                vrY.erase(ivY);
//            }
//            else
//            {
//                ivX++;
//                ivY++;
//            }
//        }
//    }
//	cout << endl;
//
//    LPFunction fr;
//    fr.setID1(ID1);
//    fr.setID2(f2.ID2);
//    fr.setUBound(uBound);
//    fr.setArrival(true);
//    fr.setValid(true);
//    fr.setValue(vrX, vrY);
//
//	cout << "In Cat:" << endl;
//	fr.display();
//
//    if(bNI)
//    {
//        fr.LPFNon_Inc(uBound);
//		cout << "Non:" << endl;
//		fr.display();
//    }
//
//	fr.display();
//    return fr;
//}
//
//void LPFunction::LPFPrune(int uBound, int rMin, int rMax, int threshold, double& uPrunePower)
//{
//    vector<int>::iterator ivX, ivY;
//
//    int x1, x2, x3, y1, y2, y3;
//    int ytmp;
//
//    int yUP = getMin() + rMax;
//    bool bPruned = false;
//    double prunePower = 1;
//    double prunePowerTmp;
//    if(vX.size() > 3)
//    {
//        for(ivX = vX.begin()+1, ivY = vY.begin()+1; ivX != vX.end()-2;)
//        {
//            if(bPruned)
//            {
//                bPruned = false;
//                ivX++;
//                ivY++;
//                continue;
//            }
//
//            x1 = *(ivX-1);
//            y1 = *(ivY-1);
//            x2 = *ivX;
//            y2 = *ivY;
//            x3 = *(ivX+1);
//            y3 = *(ivY+1);
//            ytmp = computeY(x1, x3, y1, y3, x2);
//
//            if(ytmp + rMin > yUP && y2 >= ytmp && ytmp >= threshold * y2)
//            {
//                vX.erase(ivX);
//                vY.erase(ivY);
//                bPruned = true;
//                prunePowerTmp = (double)ytmp/y2;
//                if(prunePowerTmp < prunePower)
//                    prunePower = prunePowerTmp;
//            }
//            else
//            {
//                ivX++;
//                ivY++;
//            }
//        }
//    }
//
//    uPrunePower = prunePower;
//}
//
//void LPFunction::LPFPruneSW(int uBound, int rMin, int rMax, double threshold, double& uPrunePower)
//{
//    vector<int>::iterator ivX, ivY;
//
//    int x1, x2, x3, y1, y2, y3;
//    int ytmp;
//
//    bool bPruned = false;
//    double prunePower = 1;
//    double prunePowerTmp, prunePowerTmp2;
//
//	vector<int> vXTmp, vYTmp; //buffer x,y
//	vector<int>::iterator ivXTmp, ivYTmp;
//
//    list<int> lXTmp, lYTmp;
//    list<int>::iterator ilXTmp, ilYTmp;
//
//	this->display();
//    if(vX.size() > 3)
//    {
//        x1 = vX[0];
//        y1 = vY[0];
//        bool bTerminate = false;
//		cout << "Pruning" << endl;
//        for(ivX = vX.begin()+1, ivY = vY.begin()+1; ivX != vX.end()-2;)
//        {
//            if(bTerminate)
//            {
//                x1 = *(ivX-1);
//                y1 = *(ivY-1);
//            }
//            x2 = *ivX;
//            y2 = *ivY;
//            x3 = *(ivX+1);
//            y3 = *(ivY+1);
//
//            lXTmp.push_back(x2);
//            lYTmp.push_back(y2);
//
//            prunePowerTmp = 1;
//
//            bTerminate = false;
//            bPruned = false;
//			cout << *ivX << "\t" << *ivY << endl;
//            for(ilXTmp = lXTmp.begin(), ilYTmp = lYTmp.begin(); ilXTmp != lXTmp.end() && ilYTmp != lYTmp.end(); ilXTmp++, ilYTmp++)
//            {
//                ytmp = computeY(x1, x3, y1, y3, *ilXTmp);
//                			cout << ytmp << "\t" << *ilYTmp << "\t" << threshold << "\t" << *ilYTmp*threshold << endl;
//                if(*ilYTmp >= ytmp && ytmp > (double)*ilYTmp * threshold)
//                {
//                    if(*ilXTmp == *ivX)
//                    {
//                        prunePowerTmp2 = (double)ytmp/(double)(*ilYTmp);
//                        if(prunePowerTmp2 < prunePowerTmp)
//                            prunePowerTmp = prunePowerTmp2;
//                        bPruned = true;
//                        vX.erase(ivX);
//                        vY.erase(ivY);
//                    }
//                }
//                else if(*ilYTmp < ytmp && ytmp < (double)*ilYTmp / threshold)
//                {
//                    if(*ilXTmp == *ivX)
//                    {
//                        prunePowerTmp2 = (double)(*ilYTmp)/(double)ytmp;
//                        if(prunePowerTmp2 < prunePowerTmp)
//                            prunePowerTmp = prunePowerTmp2;
//                        bPruned = true;
//                        vX.erase(ivX);
//                        vY.erase(ivY);
//                    }
//                }
//                    if(*ilYTmp < ytmp || ytmp < *ilYTmp * threshold)
//                else
//                {
//                    bTerminate = true;
//                    lXTmp.clear();
//                    lYTmp.clear();
//                    break;
//                }
//            }
//
//            if(!bPruned)
//            {
//                ivX++;
//                ivY++;
//            }
//
//            if(prunePowerTmp < prunePower)
//                prunePower = prunePowerTmp;
//        }
//    }
//cout << "after pruning" << endl;
//this->display();
//exit(0);
//    uPrunePower = prunePower;
//}
//
//LPFunction LPFunction::LPFCatArrivalInterval(LPFunction f2, int t1, int t2)
//{
//	display();
//	f2.display();
//    vector<int>::iterator ivX;
//    set<int> sX;
//    set<int>::iterator isX;
//    sX.insert(t1);
//    sX.insert(t2);
//    for(ivX = vX.begin(); ivX != vX.end(); ivX++)
//    {
//        if(*ivX > t2)
//            break;
//        if(*ivX >= t1)
//            sX.insert(*ivX);
//    }
//
//    for(ivX = f2.vX.begin(); ivX != f2.vX.end(); ivX++)
//    {
//        if(*ivX > t2)
//            break;
//        if(*ivX >= t1)
//            sX.insert(*ivX);
//    }
//
//    int x, y, f2y;
//    vector<int> vrX, vrY;
//    int xTmp = -1;
//    for(isX = sX.begin(); isX != sX.end(); isX++)
//    {
//        f2y = f2.getY(*isX);
//        x = *isX + f2y;
//        y = f2y + getY(*isX);
//        if(x <= xTmp)
//            continue;
//        xTmp = x;
//        vrX.push_back(x);
//        vrY.push_back(y);
//    }
//
//    LPFunction fr;
//    fr.setID1(ID1);
//    fr.setID2(f2.ID2);
//    fr.setUBound(upperBound);
//    fr.setArrival(true);
//    fr.setValid(true);
//    fr.setValue(vrX, vrY);
//	fr.display();
//    return fr;
//}

//remove the points larger than upperbound
//void LPFunction::LPFTrimRight()
//{
//    if (*(vX.end()-1) > upperBound)
//    {
//        vector<int>::iterator ivX, ivY;
//        vector<int> vrX, vrY;
//        for(ivX = vX.begin(), ivY = vY.begin(); ivX + 1 != vX.end(); ivX++, ivY++)
//        {
//            if(*ivX == upperBound)
//            {
//                vrX.push_back(*ivX);
//                vrY.push_back(*ivY);
//                break;
//            }
//            else if(*(ivX+1) > upperBound)
//            {
//                int y = getY(upperBound);
//                vrX.push_back(upperBound);
//                vrY.push_back(y);
//                break;
//            }
//            else
//            {
//                vrX.push_back(*ivX);
//                vrY.push_back(*ivY);
//            }
//        }
//        setValue(vrX, vrY);
//    }
//}
//
//void LPFunction::LPFTrimRight2()
//{
//    while(*(vX.end()-1) + *(vY.end()-1) > upperBound)
//    {
//        vX.erase(vX.end()-1);
//        vY.erase(vY.end()-1);
//    }
//
//    if(*(vX.end()-1) + *(vY.end()-1) < upperBound)
//    {
//        int y = *(vY.end()-1);
//        vX.push_back(upperBound - y);
//        vY.push_back(y);
//    }
//    vector<int>::iterator ivY;
//    int minY = INF;
//    for(ivY = vY.begin(); ivY != vY.end(); ivY++)
//        if(*ivY < minY)
//            minY = *ivY;
//
//    this->minY = minY;
//}

// Unconstructed
LPFunction LPFunction::LPFCat(LPFunction f2)
{
    // produce in function
//    if(bArrival)
//    {
//        vector<int>::iterator ivX, ivY, ivY1, ivY2;
//        vector<int> vrX, vrY, vf1Y, vf2X, vf2Y;
//        int m = INF;
//
//        vector<int> vXAll;
//        vXAll.reserve(vX.size() + f2.vX.size());
//
//        vector<int>::iterator ivX1, ivX2;
//
////        vector<int> vrP;
////        vector<int> vPAll;
////        vector<int>::iterator ivP1, ivP2, ivP;
////        ivP1 = vP.begin();
////        ivP2 = f2.vP.begin();
//
//        ivX2 = f2.vX.begin();
//        while(*ivX2 < *(vX.begin()))
//            ivX2++;
//
//        for(ivX1 = vX.begin(); ivX1 != vX.end() && ivX2 != f2.vX.end();) // 两个函数的x轴坐标全存到vXAll里
//        {
//            if(*ivX1 < *ivX2)
//            {
//                vXAll.push_back(*ivX1);
//                ivX1++;
//            }
//            else if(*ivX1 > *ivX2)
//            {
//                vXAll.push_back(*ivX2);
//                ivX2++;
//            }
//            else
//            {
//                vXAll.push_back(*ivX1);
//                ivX1++;
//                ivX2++;
//            }
//        }
//
//        vf1Y = getVY(vXAll); // 计算vXAll在f1上所有的Y值
//        vf2Y = f2.getVY(vXAll); // 计算vXAll在f2上所有的Y值
//
//        int x2, y2;
//        int f2xMax = *(f2.vX.end()-1);
//        vrX.reserve(vXAll.size());
//        vrY.reserve(vXAll.size());
//        // 遍历vXAll, vf1Y, vf2Y
//        for(ivX = vXAll.begin(), ivY1 = vf1Y.begin(), ivY2 = vf2Y.begin(); ivX != vXAll.end() && ivY1 != vf1Y.end() && ivY2 != vf2Y.end(); ivX++, ivY1++, ivY2++)
//        {
//            //		cout << *ivX << "\t" << *ivY1 << "\t" << *ivY2 << endl;
//            x2 = *ivX + *ivY2; // f1(f2(x)+x)：拼接后多的x值
//            y2 = *ivY1 + *ivY2;
//
//            if(x2 == f2xMax) // 如果x2等于f2的x的最大值
//            {
//                vrX.push_back(x2);
//                vrY.push_back(y2);
//                if(y2 < m) // m = INF
//                    m = y2;
//                break;
//            }
//            else if(x2 > f2xMax) // 如果x2大于f2的x的最大值，直接break
//            {
//                break;
//            }
//            else // 否则正常加入x2, y2这个新的点
//            {
//                vrX.push_back(x2);
//                vrY.push_back(y2);
////				cout << y2 << "\t" << m << endl;
//                if(y2 < m)
//                    m = y2;
////				cout << "m:" << m << endl;
//            }
////			if(m==0)
////				cout << "!!!! " << x2 << "\t" << y2;
//        }
//
//        LPFunction fr;
//        fr.setID1(ID1);
//        fr.setID2(f2.ID2);
//        fr.setArrival(true);
//        fr.setUBound(upperBound);
//        fr.setValue(vrX, vrY);
//        // setVP: let the size of vP equal to the size of fr.vX
//        vector<int> vPAll;
////      vector<int>::iterator ivP;
//        vPAll.reserve(fr.vX.size());
//        for(int i = 0; i < fr.vX.size(); i++){
//            vPAll.push_back(f2.ID1);
//        }
//        fr.setvP(vPAll);
//        return fr;
//    }
    // bArrival is always false in our application
    // produce out function
//    else
//    {
/*		cout << "In Cat:" << endl;
		display();
		f2.display();
*/
    vector<int>::iterator ivX1, ivX2, ivY1, ivY2;
    vector<int> vrX, vrY, vf1Y, vf2X, vf2Y;
    map<int, int> mX;
    map<int, int>::iterator imX;

    vector<int> vXAll;
    vXAll.reserve(vX.size() + f2.vX.size());
    vrX.reserve(vX.size() + f2.vX.size());
    vrY.reserve(vX.size() + f2.vX.size());

//        vector<int>::iterator ivP1, ivP2, ivP;
//        vector<int> vPAll;
//        ivP1 = vP.begin();

    // 获得g(t'=f()+t)的一组新的拐点的x值
    vector<int> vX2 = getXF2(f2);
/*		cout << "vX2:";
    for(ivX2 = vX2.begin(); ivX2 != vX2.end(); ivX2++)
        cout << *ivX2 << "\t";
    cout << endl;
*/

    if(vX2.empty())
        vXAll = vX;
        // 更新最终拼接后的函数所有拐点的x值
    else
    {
        for(ivX1 = vX.begin(), ivX2 = vX2.begin(); ivX1 != vX.end() && ivX2 != vX2.end();)
        {
            if(*ivX1 < *ivX2)
            {
                vXAll.push_back(*ivX1);
                ivX1++;
            }
            else if(*ivX1 > *ivX2)
            {
                vXAll.push_back(*ivX2);
                ivX2++;
            }
            else
            {
                vXAll.push_back(*ivX1);
                ivX1++;
                ivX2++;
            }
        }
    }
/*		cout << "vXAll:" ;
    for(ivX1 = vXAll.begin(); ivX1 != vXAll.end(); ivX1++)
        cout << *ivX1 << "\t";
    cout << endl;
*/
    // vXALL store the final x values of all inflection points
    // compute f(t)
    vf1Y = getVY(vXAll);

    vf2X.reserve(vXAll.size());
    // compute f(t)+t
    for(ivX1 = vXAll.begin(), ivY1 = vf1Y.begin(); ivX1 != vXAll.end() && ivY1 != vf1Y.end(); ivX1++, ivY1++){
        vf2X.push_back(*ivX1 + *ivY1);
//			cout << *ivX1 << "\t" << *ivX1 + *ivY1 << endl;
    }
    // compute g(f(t)+t)
    vf2Y = f2.getVY(vf2X);

/*		cout << "vf1X\tvf2X\t*ivY2" << endl;
    for(ivX1 = vXAll.begin(), ivX2 = vf2X.begin(), ivY2 = vf2Y.begin(); ivX1 != vXAll.end() && ivX2 != vf2X.end(); ivX1++, ivX2++, ivY2++)
    {
        cout << *ivX1 << "\t" << *ivX2 << "\t" << *ivY2 << endl;
    }
*/
    for(ivX1 = vXAll.begin(), ivY1 = vf1Y.begin(), ivY2 = vf2Y.begin(); ivX1 != vXAll.end() && ivY1 != vf1Y.end() && ivY2 != vf2Y.end(); ivX1++, ivY1++, ivY2++)
    {
        vrX.push_back(*ivX1);
        vrY.push_back(*ivY1 + *ivY2);
    }

    LPFunction fr;
    fr.setID1(ID1);
    fr.setID2(f2.ID2);
    fr.setUBound(upperBound);
    // fr.setValue(vrX, vrY);
    // cout << "vrX size:" << vrX.size() << endl;
//    fr.setArrival(false);


// Method 1:
//    cout << "vrX.size():" << vrX.size() <<"\tvrY.size():" << vrY.size() << endl;
//    fr.setValue(vrX, vrY);
//    cout << "After setting: " << endl;
//    cout << "fr.vX.size():" << fr.vX.size() << "\tfr.vY.size():" << fr.vY.size() << endl;
//
//    // setVP: let the size of vP equal to the size of fr.vX
//    vector<int> vPAll;
//    vPAll.reserve(fr.vX.size());
//    for(int i = 0; i < fr.vX.size(); i++){
//        vPAll.push_back(f2.ID1);
//    }
//    fr.setvP(vPAll);


/*    if(vrX.empty() || vrX[vrX.size()-1] > upperBound)
    {
        cout << "vrX.empty() || vrX[vrX.size()-1] > upperBound" << endl;
    }
*/
// Method 2:
// setVP: let the size of vP equal to the size of vrX
    vector<int> vPAll;
    vPAll.reserve(vrX.size());

    for(int i = 0; i < vrX.size(); i++){
        vPAll.push_back(f2.ID1);
    }
    //cout << "vPAll size:" << vPAll.size() << endl;
    fr.setValue2(vrX, vrY, vPAll);

    //cout << "fr vrX size:" << fr.vX.size() << endl;

//    if(fr.vX[fr.vX.size()-1] > upperBound || fr.vX.empty()){
//        cout << "Max x value in fr exceeds upperBound or vX is empty (in LPFCat)." << endl;
//        fr.vX = f2.vX;
//        fr.vY = f2.vY;
//        for(int i = 0; i < f2.vX.size(); i++){
//            fr.vP.push_back(f2.ID1);
//        }
//        fr.setID1(ID1);
//        fr.minY = f2.minY;
//        fr.maxY = f2.maxY;
//    }

    int ex = 0;
    if(fr.vX.empty())
    {
        //   cout << "lpf.vX is empty (in LPFCat)." << endl;
        ex = 1;
//        cout << "ex: "<< ex << endl;
//        fr.minY = INF;
//        fr.maxY = -1;
    }else if (fr.vX[fr.vX.size()-1] > upperBound)
    {
//        cout << "Max x value in lpf: " << fr.vX[fr.vX.size()-1] << endl;
//        cout << "Max x value in lpf exceeds Upperbound(in LPFCat)."<< endl;
        ex = 2;
//        cout << "ex: "<< ex << endl;
//        fr.minY = INF;
//        fr.maxY = -1;
    }
    if(ex == 1 || ex == 2)
    {
//        cout << "Max x value in lpf exceeds Upperbound or vX is empty (in LPFCat)."<< endl;
        fr.vX = f2.vX;
        fr.vY = f2.vY;
        for(int i = 0; i < f2.vX.size(); i++){
            fr.vP.push_back(f2.ID1);
        }
        fr.setID1(ID1);
        fr.minY = f2.minY;
        fr.maxY = f2.maxY;
    }

    return fr;
//	fr.display();
//    }
}

vector<int>	LPFunction::getXF2(LPFunction& f2)
{
    int x1, y1, x2, y2, x;
    vector<int> vrX;
    vector<int>::iterator ivX, ivY, ivfX;
    vrX.reserve(vX.size() + f2.vX.size());
//	display();
//	f2.display();
    for(ivX = vX.begin(), ivY = vY.begin(), ivfX = f2.vX.begin() ; ivX+1 != vX.end() && ivfX != f2.vX.end();)
    {
        if(*ivX + *ivY <= *ivfX && *(ivX+1) + *(ivY+1) >= *ivfX)
        {
            x1 = *ivX;
            y1 = *ivY;
            x2 = *(ivX+1);
            y2 = *(ivY+1);
            x = getX(x1, y1, x2, y2, *ivfX);
            if(x > 0)
                vrX.push_back(x);

            ivfX++;
        }
        else if(*ivX + *ivY > *ivfX)
            ivfX++;
        else if(*(ivX+1) + *(ivY+1) < *ivfX)
        {
            ivX++;
            ivY++;
        }
    }
    return vrX;
}

//x + f1(x) = f2x
//=> x=
int	LPFunction::getX(int x1, int y1, int x2, int y2, int f2x) const
{
    return Round(((double)(x2-x1)*(f2x-y1)+(y2-y1)*x1) / (double)(x2-x1+y2-y1));
}

//int	LPFunction::getX(int f2x)
//{
//    vector<int>::reverse_iterator ivX, ivY;
//    for(ivX = vX.rbegin(), ivY = vY.rbegin(); ivX + 1 != vX.rend(); ivX++, ivY++)
//    {
//        int x = getX(*(ivX+1), *(ivY+1), *ivX, *ivY, f2x);
//        if(x >= *(ivX+1) && x <= *ivX)
//            return x;
//    }
//    return -1;
//}

void LPFunction::display()
{
    vector<int>::iterator ivX, ivY;
    cout << "From " << ID1 << " to " << ID2 << endl << "End Points:";
    for(ivX = vX.begin(), ivY = vY.begin(); ivX != vX.end(); ivX++, ivY++)
        cout << "(" << *ivX << "," << *ivY << ")\t";
    cout << endl;
    cout << "Min Cost:" << minY << endl << endl;
}

LPFunction LPFunction::LPFMin(LPFunction f2)
{
    if(f2.vX.size() <= 2)
    {
        if(this->vX[0] > f2.vX[0])
        {
//			cout << "Insert!!!!" << endl;
            this->vX.insert(this->vX.begin(), f2.vX[0]);
            this->vY.insert(this->vY.begin(), f2.vY[0]);
        }
        return *this;
    }
    else if(this->vX.size() <= 2)
        return f2;

    if(f2.vY[1] == INF)
    {
        return *this;
    }

    if(vX[0] < f2.vX[0])
    {
        f2.vX.insert(f2.vX.begin(), vX[0]);
        f2.vY.insert(f2.vY.begin(), vY[0]);
    }
    else if(vX[0] > f2.vX[0])
    {
        vX.insert(vX.begin(), f2.vX[0]);
        vY.insert(vY.begin(), f2.vY[0]);
    }

    if(vX[vX.size()-1] < f2.vX[f2.vX.size()-1])
    {
        vX.push_back(f2.vX[f2.vX.size()-1]);
        vY.push_back(vY[vY.size()-1]);
    }
    else if(vX[vX.size()-1] > f2.vX[f2.vX.size()-1])
    {
        f2.vX.push_back(vX[vX.size()-1]);
        f2.vY.push_back(f2.vY[f2.vY.size()-1]);
    }

    vector<int> vrX, vrY;
    vector<int>::iterator ivX1, ivX2, ivY1, ivY2;
    ivX1 = vX.begin();
    ivY1 = vY.begin();
    ivX2 = f2.vX.begin();
    ivY2 = f2.vY.begin();

    int i, x, y;
    int m = min(this->minY, f2.minY);
    pair<int, int> p11, p12, p21, p22;
    int xtmp1, ytmp1, xtmp2, ytmp2;
    xtmp1 = *ivX1;
    ytmp1 = *ivY1;
    xtmp2 = *ivX2;
    ytmp2 = *ivY2;
    if(*ivX1 < *ivX2)
    {
        xtmp1 = *ivX2;
        ytmp1 = getY(xtmp1);
        while(*(ivX1+1) <= xtmp1)
        {
            ivX1++;
            ivY1++;
        }
    }
    else if(*ivX2 < *ivX1)
    {
        xtmp2 = *ivX1;
        ytmp2 = f2.getY(xtmp2);
        while(*(ivX2+1) <= xtmp2)
        {
            ivX2++;
            ivY2++;
        }
    }
    p11 = make_pair(xtmp1, ytmp1);
    p12 = make_pair(*(ivX1+1), *(ivY1+1));

    p21 = make_pair(xtmp2, ytmp2);
    p22 = make_pair(*(ivX2+1), *(ivY2+1));

    ivX1++;
    ivY1++;
    ivX2++;
    ivY2++;

    xtmp1 = vX[vX.size()-1];
    ytmp1 = vY[vY.size()-1];
    xtmp2 = f2.vX[f2.vX.size()-1];
    ytmp2 = f2.vY[f2.vY.size()-1];

    if(xtmp1 > xtmp2)
    {
        xtmp1 = xtmp2;
        vX[vX.size()-1] = xtmp1;
        vY[vY.size()-1] = getY(xtmp1);
    }
    else if(xtmp2 > xtmp1)
    {
        xtmp2 = xtmp1;
        f2.vX[f2.vX.size()-1] = xtmp2;
        f2.vY[f2.vY.size()-1] = f2.getY(xtmp2);
    }

    vector<int>::iterator ivX, ivY;

    if(p11.second <= p21.second)
    {
        vrX.push_back(p11.first);
        vrY.push_back(p11.second);
    }
    else
    {
        vrX.push_back(p21.first);
        vrY.push_back(p21.second);
    }

    for(; ivX1 != vX.end() && ivX2 != f2.vX.end();)
    {
//		if(ID1 == 110406 && ID2 == 57487)
//			if(p11.first == 37137 || p11.first == 37285)
//			if(ID1 == 69808 && ID2 == 69807)
        cout << "("<<p11.first <<","<<p11.second<<")\t(" << p12.first << "," << p12.second << ")\t(" << p21.first << "," << p21.second << ")\t(" << p22.first << "," << p22.second << ")" << endl;
/*		cout << "("<<p11.first <<","<<p11.second<<")\t(" << p12.first << "," << p12.second << ")\t(" << p21.first << "," << p21.second << ")\t(" << p22.first << "," << p22.second << ")" << endl;
		cout << "vxvy:";
		for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
			cout << "\t(" << *ivX << "," << *ivY << ")" << endl;
*/

        if(p12.first < p21.first)
        {
            p11 = p12;
            p12 = make_pair(*(ivX1+1), *(ivY1+1));
            ivX1++;
            ivY1++;
            continue;
        }
        else if(p11.first > p22.first)
        {
            p21 = p22;
            p22 = make_pair(*(ivX2+1), *(ivY2+1));
            ivX2++;
            ivY2++;
            continue;
        }

        if(p11.first>=p21.first && p12.first<=p22.first)
        {
            i = hasIntersection(p11, p12, p21, p22, x, y);
//			if(ID1 == 110406 && ID2 == 57487)
//			if(ID1 == 69808 && ID2 == 69807)
//				if(p11.first == 37137 || p11.first == 37285)
            cout << "A" << i << endl;
            if(i == 0)
            {
                int ytmp = f2.getY(p11.first);
                if(p11.second <= ytmp)
                {
                    if(p11.first > vrX[vrX.size()-1])
                    {
                        vrX.push_back(p11.first);
                        vrY.push_back(p11.second);
                    }
                }

                ytmp = f2.getY(p12.first);
                if(p12.second <= ytmp)
                {
                    if(p12.first > vrX[vrX.size()-1])
                    {
                        vrX.push_back(p12.first);
                        vrY.push_back(p12.second);
                    }
                }
                p11 = p12;
                p12.first = *(ivX1+1);
                p12.second = *(ivY1+1);
                ivX1++;
                ivY1++;

                if(ivX1 == vX.end())
                {
                    if(p22.first > vrX[vrX.size()-1])
                    {
                        //line  1 has moved forward,so p11
                        if(p22.second <= p11.second)
                        {
                            vrX.push_back(p22.first);
                            vrY.push_back(p22.second);
                        }
                        else
                        {
                            vrX.push_back(p11.first);
                            vrY.push_back(p11.second);
                        }
                    }
                }
            }
            else if(i == 1)
            {
                if(p11.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p11.first);
                    vrY.push_back(p11.second);
                }
                vrX.push_back(x);
                vrY.push_back(y);
                p11 = p12;
                p12.first = *(ivX1+1);
                p12.second = *(ivY1+1);

                p21.first = x;
                p21.second = y;
                ivX1++;
                ivY1++;
            }
            else if(i == 2)
            {

                if(p21.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p21.first);
                    vrY.push_back(p21.second);
                }

                vrX.push_back(x);
                vrY.push_back(y);
//				vrX.push_back(p12.first);
//				vrY.push_back(p12.second);

                p21.first = x;
                p21.second = y;

                p11 = p12;
                p12.first = *(ivX1+1);
                p12.second = *(ivY1+1);
                ivX1++;
                ivY1++;
            }
            else if(i == 3)
            {
                vrX.push_back(p12.first);
                vrY.push_back(p12.second);

                bool b1 = false;
                bool b2 = false;

                if(p12.first <= p22.first)
                    b1 = true;

                if(p12.first >= p22.first)
                    b2 = true;

                if(b1)
                {
                    p11 = p12;
                    p12.first = *(ivX1+1);
                    p12.second = *(ivY1+1);
                    ivX1++;
                    ivY1++;
                }

                if(b2)
                {
                    p21 = p22;
                    p22.first = *(ivX2+1);
                    p22.second = *(ivY2+1);
                    ivX2++;
                    ivY2++;
                }
            }
        }
        else if(p11.first<=p21.first && p12.first>=p22.first)
        {
            i = hasIntersection(p11, p12, p21, p22, x, y);
//			if(ID1 == 69808 && ID2 == 69807)
//			if(ID1 == 110406 && ID2 == 57487)
//				if(p11.first == 37137 || p11.first == 37285)
            cout << "B" << i << endl;
            if(i == 0)
            {
                int ytmp;
                if(p11.first > vrX[vrX.size()-1])
                {
                    ytmp = f2.getY(p11.first);
                    if(ytmp >= p11.second)
                    {
                        vrX.push_back(p11.first);
                        vrY.push_back(p11.second);
                    }
                }

                ytmp = getY(p21.first);
                if(p21.second <= ytmp)
                {
                    if(p21.first > vrX[vrX.size()-1])
                    {
                        vrX.push_back(p21.first);
                        vrY.push_back(p21.second);
                    }
                }

/*				ytmp = getY(p22.first);
				if(p22.second <= ytmp)
				{
					if(p22.first > vrX[vrX.size()-1])
					{
						vrX.push_back(p22.first);
						vrY.push_back(p22.second);
					}
				}*/

                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
                if(ivX2 == f2.vX.end())
                {
                    if(p12.first > vrX[vrX.size()-1])
                    {
                        if(p12.second <= p21.second)
                        {
                            vrX.push_back(p12.first);
                            vrY.push_back(p12.second);
                        }
                        else
                        {
                            vrX.push_back(p21.first);
                            vrY.push_back(p21.second);
                        }
                    }
                }
            }
            else if(i == 2)
            {
                if(p21.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p21.first);
                    vrY.push_back(p21.second);
                }
                vrX.push_back(x);
                vrY.push_back(y);

                p11.first = x;
                p11.second = y;

                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
            }
            else if(i == 1)
            {
                int ytmp;
                if(p11.first > vrX[vrX.size()-1])
                {
                    ytmp = f2.getY(p11.first);
                    if(ytmp >= p11.second)
                    {
                        vrX.push_back(p11.first);
                        vrY.push_back(p11.second);
                    }
                }

                vrX.push_back(x);
                vrY.push_back(y);
//				vrX.push_back(p22.first);
//				vrY.push_back(p22.second);

                p11.first = x;
                p11.second = y;

                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
            }
            else if(i == 3)
            {
                vrX.push_back(p12.first);
                vrY.push_back(p12.second);
                bool b1 = false;
                bool b2 = false;

                if(p12.first <= p22.first)
                    b1 = true;

                if(p12.first >= p22.first)
                    b2 = true;

                if(b1)
                {
                    p11 = p12;
                    p12.first = *(ivX1+1);
                    p12.second = *(ivY1+1);
                    ivX1++;
                    ivY1++;
                }

                if(b2)
                {
                    p21 = p22;
                    p22.first = *(ivX2+1);
                    p22.second = *(ivY2+1);
                    ivX2++;
                    ivY2++;
                }

            }
        }
        else if(p11.first<=p21.first && p12.first>=p21.first)
        {
            i = hasIntersection(p11, p12, p21, p22, x, y);
//			if(ID1 == 110406 && ID2 == 57487)
//				if(p11.first == 37137 || p11.first == 37285)
//			if(ID1 == 69808 && ID2 == 69807)
            cout << "C" << i << endl;
            if(i == 0)
            {
                int ytmp = getY(p21.first);
                if(p21.second < ytmp)
                {
                    if(p21.first > vrX[vrX.size()-1])
                    {
                        vrX.push_back(p21.first);
                        vrY.push_back(p21.second);
                    }
                }

                if(p22.first <= p12.first)
                {
                    ytmp = getY(p22.first);
                    if(p22.second <= ytmp)
                    {
                        vrX.push_back(p22.first);
                        vrY.push_back(p22.second);
                    }
                    p21 = p22;
                    p22.first = *(ivX2+1);
                    p22.second = *(ivY2+1);
                    ivX2++;
                    ivY2++;
                }
                else
                {
                    p11 = p12;
                    p12.first = *(ivX1+1);
                    p12.second = *(ivY1+1);
                    ivX1++;
                    ivY1++;
                }
            }
            else if(i == 1)
            {
                if(p11.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p11.first);
                    vrY.push_back(p11.second);
                }
                vrX.push_back(p11.first);
                vrY.push_back(p11.second);
                vrX.push_back(x);
                vrY.push_back(y);

                p21.first = x;
                p21.second = y;

                p11 = p12;
                p12.first = *(ivX1+1);
                p12.second = *(ivY1+1);

                ivX1++;
                ivY1++;
            }
            else if(i == 2)
            {
                if(p21.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p21.first);
                    vrY.push_back(p21.second);
                }
                vrX.push_back(x);
                vrY.push_back(y);

                p21.first = x;
                p21.second = y;

                p11 = p12;
                p12.first = *(ivX1+1);
                p12.second = *(ivY1+1);
                ivX1++;
                ivY1++;
            }
            else if(i == 3)
            {
                vrX.push_back(p12.first);
                vrY.push_back(p12.second);
                bool b1 = false;
                bool b2 = false;

                if(p12.first <= p22.first)
                    b1 = true;

                if(p12.first >= p22.first)
                    b2 = true;

                if(b1)
                {
                    p11 = p12;
                    p12.first = *(ivX1+1);
                    p12.second = *(ivY1+1);
                    ivX1++;
                    ivY1++;
                }

                if(b2)
                {
                    p21 = p22;
                    p22.first = *(ivX2+1);
                    p22.second = *(ivY2+1);
                    ivX2++;
                    ivY2++;
                }

            }
        }
        else if(p11.first<=p22.first && p12.first>=p22.first)
        {
            i = hasIntersection(p11, p12, p21, p22, x, y);
//			if(ID1 == 110406 && ID2 == 57487)
//				if(p11.first == 37137 || p11.first == 37285)
//			if(ID1 == 69808 && ID2 == 69807)
            cout << "D" << i << endl;
            if(i == 0)
            {
                int ytmp = getY(p21.first);
                if(p21.second < ytmp)
                {
                    if(p21.first > vrX[vrX.size()-1])
                    {
                        vrX.push_back(p21.first);
                        vrY.push_back(p21.second);
                    }
                }
/*				else if(p11.second > p22.second)
				{
					if(p22.first > vrX[vrX.size()-1])
					{
						vrX.push_back(p22.first);
						vrY.push_back(p22.second);
					}
				}*/
                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
            }
            else if(i == 2)
            {
                vrX.push_back(x);
                vrY.push_back(y);

                p11.first = x;
                p11.second = y;

                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
            }
            else if(i == 1)
            {
                if(p11.first > vrX[vrX.size()-1])
                {
                    vrX.push_back(p11.first);
                    vrY.push_back(p11.second);
                }
                vrX.push_back(x);
                vrY.push_back(y);

                vrX.push_back(p22.first);
                vrY.push_back(p22.second);

                p11.first = x;
                p11.second = y;

                p21 = p22;
                p22.first = *(ivX2+1);
                p22.second = *(ivY2+1);
                ivX2++;
                ivY2++;
            }
            else if(i == 3)
            {
                vrX.push_back(p12.first);
                vrY.push_back(p12.second);

                bool b1 = false;
                bool b2 = false;

                if(p12.first <= p22.first)
                    b1 = true;

                if(p12.first >= p22.first)
                    b2 = true;

                if(b1)
                {
                    p11 = p12;
                    p12.first = *(ivX1+1);
                    p12.second = *(ivY1+1);
                    ivX1++;
                    ivY1++;
                }

                if(b2)
                {
                    p21 = p22;
                    p22.first = *(ivX2+1);
                    p22.second = *(ivY2+1);
                    ivX2++;
                    ivY2++;
                }
            }
        }
    }

    cout << "IN MIn:" << endl;
    vector<int> vrrX,vrrY;
    for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
    {
        if((ivX != vrX.begin() && *ivX > *(ivX-1)) || ivX == vrX.begin())
        {
            cout << "(" <<  *ivX << "," << *ivY << "),";
            vrrX.push_back(*ivX);
            vrrY.push_back(*ivY);
        }
    }
    cout << endl;

    if(vrrX[vrrX.size()-1] < vX[vX.size()-1])
    {
        vrrX.push_back(vX[vX.size()-1]);
        if(vY[vY.size()-1] < f2.vY[f2.vY.size()-1])
            vrrY.push_back(vY[vY.size()-1]);
        else
            vrrY.push_back(f2.vY[f2.vY.size()-1]);
    }

    LPFunction fr;
    fr.setID1(getID1());
    fr.setID2(getID2());
    fr.setUBound(getUBound());
//    fr.setArrival(true);
    fr.setValue(vrrX, vrrY);
    fr.setMin(m);

    return fr;
}

void LPFunction::swapLine(pair<int, int>& p11, pair<int, int>& p12, pair<int, int>& p21, pair<int, int>& p22)
{
    pair<int, int> pTmp;
    pTmp = p11;
    p11 = p21;
    p21 = pTmp;

    pTmp = p12;
    p12 = p22;
    p22 = pTmp;
}

//0: no intersection
//1: p11 and xy safe
//2: p21 and xy safe
//3: on the same line
int LPFunction::hasIntersection(pair<int, int> p11, pair<int, int> p12, pair<int, int> p21, pair<int, int> p22, int& x, int& y)
{
    double d1 = direction(p21, p22, p11);
    double d2 = direction(p21, p22, p12);
    double d3 = direction(p11, p12, p21);
    double d4 = direction(p11, p12, p22);

    int x11 = p11.first;
    int y11 = p11.second;
    int x12 = p12.first;
    int y12 = p12.second;
    int x21 = p21.first;
    int y21 = p21.second;
    int x22 = p22.first;
    int y22 = p22.second;
    if(y11 == y12 && y21 == y22 && y11 == y21) //两条线平行
        return 3;
//	cout << "direction:" << d1 << "\t" << d2 << "\t" << d3 << "\t" << d4 <<endl;
    if(d1>=0 && d2<0 && d3<0 && d4>0 )
    {
//		cout << "d1>0" << endl;
        double k1=(double)(y11-y12)/(x11-x12);
        double k2=(double)(y21-y22)/(x21-x22);
        x = Round((k1*x11 - k2*x21 +y21 - y11) / (k1-k2));
        y = Round(y11 + (x-x11)*k1);

        if(x < 0 || y < 0)
            return 0;

//		cout << x << "\t" << y << endl;
        if(onSegment(p11, p12, std::make_pair(x,y)) && onSegment(p21, p22, std::make_pair(x,y)))
            return 1;
        else
            return 0;
    }
    else if(d1<=0 && d2>0 && d3>0 && d4<0)
    {
//		cout << "d1<0" << endl;
        double k1=(double)(y11-y12)/(x11-x12);
        double k2=(double)(y21-y22)/(x21-x22);
        x = Round((k1*x11 - k2*x21 +y21 - y11) / (k1-k2));
        y = Round(y11 + (x-x11)*k1);
        if(x < 0 || y < 0)
        {
            return 0;
        }

        if(onSegment(p11, p12, std::make_pair(x,y)) && onSegment(p21, p22, std::make_pair(x,y)))
            return 2;
        else
            return 0;
    }
    else if(d1==0 && d2==0 && d3==0 && d4==0)	//Same line
        return 3;
    else
        return 0;
}

double LPFunction::direction(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk)
{
    return (pk.first-pi.first) * (pj.second-pi.second) - (pj.first-pi.first) * (pk.second-pi.second);
}

bool LPFunction::Segment(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk)
{
    if(min(pi.first, pj.first) < pk.first < max(pi.first, pj.first) && min(pi.second, pj.second) <= pk.second <= max(pi.second, pj.second))
        return true;
    else
        return false;

}
bool LPFunction::onSegment(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk)
{
    if(min(pi.first, pj.first) < pk.first < max(pi.first, pj.first) && min(pi.second, pj.second) <= pk.second <= max(pi.second, pj.second))
        return true;
    else
        return false;

}

int LPFunction::getMin()
{
    return minY;
}

int LPFunction::getMax()
{
    return maxY;
}

void LPFunction::setMin(int m)
{
    minY = m;
}

bool LPFunction::dominate(LPFunction f2)
{
    vector<int>::iterator ivY, ivYtmp;
    vector<int> vYtmp;
    if(ID1 == -1 || ID2 == -1)
        return false;

    // if this->lpf 为空
    if(vX.size() < 2)
        return false;
        // if f2->lpf 为空
    else if(f2.vX.size() < 2)
        return true;

    vector<int>::iterator ivX;

//	cout << "In Dominate" << endl;
    vYtmp = f2.getVY(vX);

    ivYtmp = vYtmp.begin();
    ivY = vY.begin();
    ivX = vX.begin();

    for(;ivX != vX.end() && ivY != vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivY > *ivYtmp + 10)
        {
            return false;
        }
    }

    vYtmp.clear();

    vYtmp = getVY(f2.vX);

    ivYtmp = vYtmp.begin();
    ivY = f2.vY.begin();
    ivX = f2.vX.begin();

    for(ivY = f2.vY.begin(), ivYtmp = vYtmp.begin(), ivX = f2.vX.begin(); ivY != f2.vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivYtmp > *ivY + 10)
        {
            return false;
        }
    }

    return true;
}

bool LPFunction::dominateDisplay(LPFunction f2)
{
    vector<int>::iterator ivY, ivYtmp;
    vector<int> vYtmp;
    if(ID1 == -1 || ID2 == -1)
        return false;

    if(vX.size() < 2)
        return false;
    else if(f2.vX.size() < 2)
        return true;

    vector<int>::iterator ivX;

//	cout << "In Dominate" << endl;
    vYtmp = f2.getVY(vX);

    ivY = vY.begin();
    ivYtmp = vYtmp.begin();

    ivX = vX.begin();

    for(;ivX != vX.end() && ivY != vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivY > *ivYtmp + 10)
        {
            cout << "a:" << *ivX << "\t" << *ivY << "\t" << *ivYtmp << endl;
            return false;
        }
    }

    vYtmp.clear();
    vYtmp = getVY(f2.vX);

    ivY = f2.vY.begin();
    ivYtmp = vYtmp.begin();
    ivX = f2.vX.begin();

    for(ivY = f2.vY.begin(), ivYtmp = vYtmp.begin(), ivX = f2.vX.begin(); ivY != f2.vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivYtmp > *ivY + 10)
        {
            cout << "b:" << *ivX << "\t" << *ivY << "\t" << *ivYtmp << endl;
            return false;
        }
    }

    return true;
}

bool LPFunction::dominate5(LPFunction f2)
{
    vector<int>::iterator ivY, ivYtmp;
    vector<int> vYtmp;
    if(vX.size() < 2)
        return false;
    else if(f2.vX.size() < 2)
        return true;

    vector<int>::iterator ivX;

//	cout << "In Dominate" << endl;
    vYtmp = f2.getVY(vX);

    ivY = vY.begin();
    ivYtmp = vYtmp.begin();

    ivX = vX.begin();

    for(;ivX != vX.end() && ivY != vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivY > *ivYtmp + 5)
        {
            return false;
        }
    }

    vYtmp.clear();
    vYtmp = getVY(f2.vX);

    ivY = f2.vY.begin();
    ivYtmp = vYtmp.begin();
    ivX = f2.vX.begin();

    for(ivY = f2.vY.begin(), ivYtmp = vYtmp.begin(), ivX = f2.vX.begin(); ivY != f2.vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivYtmp > *ivY + 5)
        {
            return false;
        }
    }

    return true;
}

bool LPFunction::dominate2(LPFunction f2)
{
    vector<int>::iterator ivY, ivYtmp;
    vector<int> vYtmp;
    if(vX.size() < 2)
        return false;
    else if(f2.vX.size() < 2)
        return true;

    vector<int>::iterator ivX;

//	cout << "In Dominate" << endl;
    vYtmp = f2.getVY(vX);

    ivY = vY.begin();
    ivYtmp = vYtmp.begin();

    ivX = vX.begin();

    for(;ivX != vX.end() && ivY != vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivY > *ivYtmp + 2)
        {
            return false;
        }
    }

    vYtmp.clear();
    vYtmp = getVY(f2.vX);

    ivY = f2.vY.begin();
    ivYtmp = vYtmp.begin();
    ivX = f2.vX.begin();

    for(ivY = f2.vY.begin(), ivYtmp = vYtmp.begin(), ivX = f2.vX.begin(); ivY != f2.vY.end() && ivYtmp != vYtmp.end(); ivY++, ivYtmp++, ivX++)
    {
        if(*ivX + *ivY >= upperBound || *ivX + *ivYtmp >= upperBound)
            break;

        if(*ivYtmp > *ivY + 2)
        {
            return false;
        }
    }

    return true;
}

string LPFunction::toString()
{
    string outString;
    stringstream ss;
    string sID1, sID2, sUpperBound, sMinY, sSize;

    ss << ID1;
    ss >> sID1;
    ss.clear();
    ss.str("");

    ss << ID2;
    ss >> sID2;
    ss.clear();
    ss.str("");

    ss << upperBound;
    ss >> sUpperBound;
    ss.clear();
    ss.str("");

    ss << minY;
    ss >> sMinY;
    ss.clear();
    ss.str("");

    ss << vX.size();
    ss >> sSize;
    ss.clear();
    ss.str("");


    outString = sID1 + "\t" + sID2 + "\t" + sUpperBound + "\t" + sMinY + "\t" + sSize + "\t";

    vector<int>::iterator ivX, ivY;
    string sx, sy;
    for(ivX = vX.begin(), ivY = vY.begin(); ivX != vX.end(); ivX++, ivY++)
    {
        ss << *ivX;
        ss >> sx;
        ss.clear();
        ss.str("");

        ss << *ivY;
        ss >> sy;
        ss.clear();
        ss.str("");

        outString += sx + "\t" + sy + "\t";
    }
    outString += "\n";

    return outString;
}

void LPFunction::setLast(int y)
{
    if(vX.size() > 2 && vY[vY.size()-1] == INF)
        vY[vY.size()-1] = y;

}


int	LPFunction::getX2(int x1, int y1, int x2, int y2, int y)
{
    return x1 + (y-y2)*(x2-x1)/(y2-y1);
}

//LPFunction LPFunction::LPFMinNew(LPFunction f2)
//{
//    if(f2.vX.size() <= 2)
//    {
//        if(this->vX[0] > f2.vX[0])
//        {
//            cout << "Insert!!!!" << endl;
//            this->vX.insert(this->vX.begin(), f2.vX[0]);
//            this->vY.insert(this->vY.begin(), f2.vY[0]);
//        }
//        return *this;
//    }
//    else if(this->vX.size() <= 2)
//        return f2;
//
//    if(f2.vY[1] == INF)
//    {
//        return *this;
//    }
//
//    if(vX[0] < f2.vX[0])
//    {
//        f2.vX.insert(f2.vX.begin(), vX[0]);
//        f2.vY.insert(f2.vY.begin(), vY[0]);
//    }
//    else if(vX[0] > f2.vX[0])
//    {
//        vX.insert(vX.begin(), f2.vX[0]);
//        vY.insert(vY.begin(), f2.vY[0]);
//    }
//
//    if(vX[vX.size()-1] < f2.vX[f2.vX.size()-1])
//    {
//        vX.push_back(f2.vX[f2.vX.size()-1]);
//        vY.push_back(vY[vY.size()-1]);
//    }
//    else if(vX[vX.size()-1] > f2.vX[f2.vX.size()-1])
//    {
//        f2.vX.push_back(vX[vX.size()-1]);
//        f2.vY.push_back(f2.vY[f2.vY.size()-1]);
//    }
//
//    int i,j,is;
//    vector<int> vrX, vrY;
//    int x,y;
//    for(i = 0, j = 0; i < (int)f2.vX.size() && j < (int)vX.size();)
//    {
////		cout << "i:" << i << "\tj:" << j << endl;
////		cout << f2.vX[i-1] << "\t" << vX[j] << "\t" << f2.vY[i] << "\t" << vY[j] << endl;
//        if(i >0 && j > 0)
//            cout << f2.vX[i-1] << "," <<  f2.vY[i-1] << "\t" << f2.vX[i] << "," << f2.vY[i]  << "\t" << vX[j-1] << "," << vY[j-1] << "\t" << vX[j] << "," <<  vY[j] << endl;
//
//        if(f2.vX[i] == vX[j])
//        {
//            if(i > 0 && j > 0)
//            {
//                if(f2.vX[i-1] >= vX[j-1])
//                {
//                    int ytmp = f2.getY(vX[j-1]);
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        cout << "111 a " << ytmp << "\t" << vY[j-1] << endl;
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//
//                    ytmp = getY(f2.vX[i-1]);
//                    if(ytmp > f2.vY[i-1] && f2.vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        cout << "111 b " << ytmp << "\t" << f2.vY[i-1] << endl;
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//                }
//
//                else if(f2.vX[i-1] < vX[j-1])
//                {
//                    int ytmp = getY(f2.vX[i-1]);
//                    if(ytmp > f2.vY[i-1] && f2.vX[i-1] > vrX[vrX.size()-1])
//                    {
//                        cout << "111 c " << ytmp << "\t" << f2.vY[i-1] << endl;
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//
//                    ytmp = f2.getY(vX[j-1]);
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        cout << "111 d " << ytmp << "\t" << vY[j-1] << endl;
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//                }
//            }
//
//            cout << "111 bUpdated" << endl;
//            if(f2.vY[i] < vY[j])
//            {
//                if(vrX.size() > 1 && vrX[vrX.size()-1] == f2.vX[i])
//                {
//                    if(f2.vY[i] < vrY[vrY.size() - 1])
//                    {
//                        vrY[vrY.size()-1] = min(f2.vY[i], vrY[vrY.size() - 1]);
//                    }
//                }
//                else
//                {
//                    vrX.push_back(f2.vX[i]);
//                    vrY.push_back(f2.vY[i]);
//                }
//            }
//            else
//            {
//                if(vrX.size() > 1 && vrX[vrX.size()-1] == f2.vX[i])
//                {
//                    vrY[vrY.size()-1] = min(vY[j], vrY[vrY.size() - 1]);
//                }
//                else
//                {
//                    vrX.push_back(f2.vX[i]);
//                    vrY.push_back(vY[j]);
//                }
//            }
//            i++;
//            j++;
//        }
//        else if(f2.vX[i] < vX[j])
//        {
//            if(j == 0)
//            {
//                i++;
//                continue;
//            }
//            is = hasIntersection(make_pair(f2.vX[i-1], f2.vY[i-1]), make_pair(f2.vX[i], f2.vY[i]), make_pair(vX[j-1], vY[j-1]), make_pair(vX[j], vY[j]), x, y);
////			is = hasIntersection(vNewX[i-1], vNewY[i-1], vNewX[i], vNewY[i], vvX[neighborNodeID][j-1], vvY[neighborNodeID][j-1], vvX[neighborNodeID][j], vvY[neighborNodeID][j], x, y);
//            if(is != 0)
//                cout << is << "\t" << x << "\t" << y << endl;
//
//            if(i > 0 && j > 0)
//            {
//                if(f2.vX[i-1] < vX[j-1])
//                {
//                    int ytmp = getY(f2.vX[i-1]);
//                    cout << "222 a " << ytmp << "\t" << f2.vY[i-1] << endl;
//                    if(ytmp > f2.vY[i-1] && f2.vX[i-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//
//                    ytmp = f2.getY(vX[j-1]);
//                    cout << "222 b " << ytmp << "\t" << vY[j-1] << endl;
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//
//                }
//                else if(f2.vX[i-1] >= vX[j-1])
//                {
//                    int ytmp = f2.getY(vX[j-1]);
//                    cout << "222 c " << ytmp << "\t" << vY[j-1] << endl;
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//
//                    ytmp = getY(f2.vX[i-1]);
//                    cout << "222 d " << ytmp << "\t" << f2.vY[i-1] << endl;
//                    if(ytmp > f2.vY[i-1] && f2.vX[i-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//                }
//            }
////			if(x >0 && y>0)
////			if(is)
////				cout <<vNewX[i-1] << "\t" << vNewY[i-1] << "\t" << vNewX[i] << "\t" << vNewY[i] << "\t" << vvX[neighborNodeID][j-1] << "\t" <<  vvY[neighborNodeID][j-1] << "\t" <<  vvX[neighborNodeID][j] <<"\t" << vvY[neighborNodeID][j] << "\t" << x <<"\t" << y << endl;
///*			if(y < minCost)
//			{
//				cout << "$$$$y:" << y << endl;
//				cout << vvX[neighborNodeID][j-1] << "\t" <<  vvY[neighborNodeID][j-1] << "\t" <<  vvX[neighborNodeID][j] <<"\t" << vvY[neighborNodeID][j] << "\t" << vNewX[i-1] << "\t" << vNewY[i-1] << "\t" << vNewX[i] << "\t" << vNewY[i] << "\t" << "\t" << "\t" << x <<"\t" << y << endl;
//				minCost = y;
//			}*/
//            if(is == 1)
//            {
//                cout << "222 bUpdated" << endl;
//                if(x == vrX[vrX.size()-1])
//                {
////					vX.push_back(x);
//                    vrY[vrY.size()-1] = min(y, f2.vY[i]);
//                }
//                else
//                {
//                    //		vX.push_back(vNewX[i]);
//                    //		vY.push_back(vNewY[i]);
//                    cout << "Push:" << x << "\t" << y << endl;
//                    if(vrX[vrX.size()-1] < f2.vX[i-1])
//                    {
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//                    vrX.push_back(x);
//                    vrY.push_back(y);
//                }
//            }
//            else if(is == 2)
//            {
//                cout << "333 bUpdated" << endl;
//                if(vrX.size() > 1)
//                {
//                    if(x == vrX[vrX.size()-1])
//                    {
//                        vrY[vrY.size()-1] = min(y, vrY[vrY.size()-1]);
//                    }
//                    else
//                    {
//                        vrX.push_back(x);
//                        vrY.push_back(y);
//                    }
//                }
//                else
//                {
//                    vrX.push_back(x);
//                    vrY.push_back(y);
//                }
//            }
//            else if(is == 0)
//            {
////				cout << "Do not intersect" << endl;
//
//                int ytmp = getY(f2.vX[i]);
//                if(f2.vY[i] < ytmp)
//                {
//                    cout << "444 bUpdated" << endl;
//                    vrX.push_back(f2.vX[i]);
//                    vrY.push_back(f2.vY[i]);
//                }
///*
//				if(f2.vY[i] < vY[j])
//				{
//					cout << "444 bUpdated" << endl;
//					if(vX.size() > 1)
//					{
//						if(vrX[vrX.size()-1]==f2.vX[i])
//						{
//							vrY[vrY.size()-1] = min(f2.vY[i], vrY[vrY.size()-1]);
//						}
//						else
//						{
//							vrX.push_back(f2.vX[i]);
//							vrY.push_back(f2.vY[i]);
//						}
//					}
//					else
//					{
//						vrX.push_back(f2.vX[i]);
//						vrY.push_back(f2.vY[i]);
//					}
//				}
//*/			}
//            i++;
//        }
//        else if(f2.vX[i] > vX[j])
//        {
//            if(j == 0)
//            {
//                i++;
//                continue;
//            }
//
//
////			is = hasIntersection(vvX[neighborNodeID][j-1], vvY[neighborNodeID][j-1], vvX[neighborNodeID][j], vvY[neighborNodeID][j], vNewX[i-1], vNewY[i-1], vNewX[i], vNewY[i], x, y);
//            is = hasIntersection(make_pair(vX[j-1], vY[j-1]), make_pair(vX[j], vY[j]), make_pair(f2.vX[i-1], f2.vY[i-1]), make_pair(f2.vX[i], f2.vY[i]), x, y);
//            if(is != 0)
//                cout << is << "\t" << x << "\t" << y << endl;
////				if(is)
////			cout <<vNewX[i-1] << "\t" << vNewY[i-1] << "\t" << vNewX[i] << "\t" << vNewY[i] << "\t" << vvX[neighborNodeID][j-1] << "\t" <<  vvY[neighborNodeID][j-1] << "\t" <<  vvX[neighborNodeID][j] <<"\t" << vvY[neighborNodeID][j] << "\t" << x <<"\t" << y << endl;
//
//            if(i > 0 && j > 0)
//            {
//                if(f2.vX[i-1] >= vX[j-1])
//                {
//                    int ytmp = f2.getY(vX[j-1]);
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//
//                    ytmp = getY(f2.vX[i-1]);
//                    if(ytmp > f2.vY[i-1] && f2.vX[i-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//                }
//                else if(f2.vX[i-1] < vX[j-1])
//                {
//                    int ytmp = getY(f2.vX[i-1]);
//                    if(ytmp > f2.vY[i-1] && f2.vX[i-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(f2.vX[i-1]);
//                        vrY.push_back(f2.vY[i-1]);
//                    }
//
//                    ytmp = f2.getY(vX[j-1]);
//                    if(ytmp > vY[j-1] && vX[j-1] > vrX[vrX.size()-1])
//                    {
//                        vrX.push_back(vX[j-1]);
//                        vrY.push_back(vY[j-1]);
//                    }
//                }
//            }
//
//            if(is == 1)
//            {
//                cout << "555 bUpdated" << endl;
//                if(x == vrX[vrX.size()-1])
//                {
//                    vrY[vrY.size()-1] =  min(y, vY[j]);
//                }
//                else
//                {
//                    //			vX.push_back(vvX[neighborNodeID][j]);
//                    //			vY.push_back(vvY[neighborNodeID][j]);
//                    vrX.push_back(x);
//                    vrY.push_back(y);
//                }
//            }
//            else if(is == 2)
//            {
//                cout << "666 bUpdate = true" << endl;
//                if(vrX.size() > 1)
//                {
//                    if(x == vrX[vrX.size()-1])
//                    {
//                        vrY[vrY.size()-1] = min(y, vrY[vrY.size()-1]);
//                    }
//                    else
//                    {
//                        vrX.push_back(x);
//                        vrY.push_back(y);
//                    }
//                }
//                else
//                {
//                    vrX.push_back(x);
//                    vrY.push_back(y);
//                }
//            }
//            else if(is == 0)
//            {
//                int ytmp = f2.getY(vX[j]);
//                if(vY[j] < ytmp)
//                {
//                    cout << "777 bUpdate = true" << endl;
//                    vrX.push_back(vX[j]);
//                    vrY.push_back(vY[j]);
//                }
//
///*				if(f2.vY[i] < vY[j])
//				{
//					cout << "777 bUpdate = true" << endl;
//					if(vrX.size() > 1)
//					{
//						if(vrX[vrX.size()-1]==f2.vX[i])
//						{
//							vrY[vrY.size()-1] = min(f2.vY[i], vrY[vrY.size()-1]);
//						}
//					}
//					else
//					{
//						vrX.push_back(f2.vX[j]);
//						vrY.push_back(f2.vY[i]);
//					}
//				}
//				else
//				{
//					vrX.push_back(vX[j]);
//					vrY.push_back(vY[j]);
//				}
//*/			}
//            j++;
//        }
//    }
//
//    vector<int> vrrX,vrrY;
//    vector<int>::iterator ivX, ivY;
//    int m = INF, m2 = INF;
//    for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
//    {
//        if((ivX != vrX.begin() && *ivX > vrrX[vrrX.size()-1]) || ivX == vrX.begin())
//        {
//            cout << "(" <<  *ivX << "," << *ivY << "),";
////			if(*ivX > LD)
////				break;
//            vrrX.push_back(*ivX);
//            vrrY.push_back(*ivY);
//            if(*ivY < m)
//            {
//                m2 = m;
//                m = *ivY;
//            }
//        }
//    }
////	cout << endl;
//
//    if(vrrX[vrrX.size()-1] < vX[vX.size()-1])
//    {
//        vrrX.push_back(vX[vX.size()-1]);
//        if(vY[vY.size()-1] < f2.vY[f2.vY.size()-1])
//            vrrY.push_back(vY[vY.size()-1]);
//        else
//            vrrY.push_back(f2.vY[f2.vY.size()-1]);
//    }
//
//    if(vrrY[vrrY.size()-2] - vrrY[vrrY.size()-1] > 100)
//    {
//        if(m == vrrY[vrrY.size()-1])
//            m = m2;
//        vrrY[vrrY.size()-1] = vrrY[vrrY.size()-2];
//    }
//
//
//    LPFunction fr;
//    fr.setID1(getID1());
//    fr.setID2(getID2());
//    fr.setUBound(getUBound());
//    fr.setArrival(true);
//    fr.setValue(vrrX, vrrY);
//    fr.setMin(m);
//
//    return fr;
//}

LPFunction LPFunction::LPFMinNew3(LPFunction f2)
{
/*	cout << "F1:" << endl;
	display();
	cout << "F2:"  << endl;
	f2.display();
*/
    // true: f1 is smaller
    // false: f2 is smaller
    bool b = true;

    if(this->getSize() == 1)
        return f2;
    if(f2.getSize() == 1)
        return *this;

    // ID1 == -1: 不连通，lpf为空
    if(this->getID1() == -1)
        return f2;
    if(f2.getID1() == -1)
        return *this;

    list<int> lXTmp, lYTmp;
    list<int>::iterator ilXTmp, ilYTmp;
    // 记录什么的？
    int nXTmp = 0;
    int nYTmp = 0;

    if(f2.vY[1] == INF)
        return *this;

    vector<int>::iterator ivX1, ivX2, ivY1, ivY2;
    ivX1 = vX.begin();
    ivY1 = vY.begin();
    ivX2 = f2.vX.begin();
    ivY2 = f2.vY.begin();

    int m = INF;

    vector<int> vrX, vrY;
    vrX.reserve(vX.size() + f2.vX.size());
    vrY.reserve(vY.size() + f2.vY.size());

    // Variables for vP
    list<int> lPTmp;
    list<int>::iterator ilPTmp;
    int nPTmp = 0;
    vector<int>::iterator ivP1, ivP2;
    ivP1 = vP.begin();
    ivP2 = f2.vP.begin();
    vector<int> vrP;
    vrP.reserve(vP.size() + f2.vP.size());

    // Variable b
    // true: f1 is smaller
    // false: f2 is smaller
    if(*ivY1 <= *ivY2){
        b = true;
        vrX.push_back(*ivX1);
        vrY.push_back(*ivY1);
    }
    else{
        b = false;
        vrX.push_back(*ivX2);
        vrY.push_back(*ivY2);
    }

    int x = *ivX1;
    int y1, y2;

    pair<int,int> p11, p12, p21, p22;
    int ix, iy;

    int x11, x12, x21, x22, y11, y12, y21, y22;
    while(ivX1+1 != vX.end() && ivX2+1 != f2.vX.end())
    {
        x11 = *ivX1;
        x21 = *ivX2;
        x12 = *(ivX1+1);
        x22 = *(ivX2+1);

        y11 = *ivY1;
        y21 = *ivY2;
        y12 = *(ivY1+1);
        y22 = *(ivY2+1);


        if(ivX1+1 == vX.end()){
        }

        if(ivX2+1 == f2.vX.end()){
        }

        if(*(ivX1+1) < *(ivX2+1))
        {
            y1 = *(ivY1+1);
            y2 = f2.getY(*(ivX1+1));
            if(b) // *ivY1 <= *ivY2
            {
//				cout << "A1 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 <= y2){
                    vrX.push_back(*(ivX1+1));
                    vrY.push_back(y1);
                    // f2完全在f1的上方
                    vrP.push_back(*(ivP1+1));
                }
                else{ // y1 > y2
                    p11 = make_pair(x11, y11);
                    p12 = make_pair(x12, y12);
                    p21 = make_pair(x21, y21);
                    p22 = make_pair(x22, y22);
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 0)
                    {
                        vrX.push_back(*(ivX1+1));
                        vrY.push_back(y2);
                        vrP.push_back(*(ivP1+1));
                    }
                    else if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1] && ix <= x12)
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                        }
                        b = false; // f2 is smaller

                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }

                    }
                    else // it == 3
                    {
                        vrX.push_back(*(ivX1+1));
                        vrY.push_back(y1);
                        vrP.push_back(*(ivP1+1));
                    }
                }
            }
            else // b = false, *ivY1 > *ivY2
            {
//				cout << "A2 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 <= y2)
                {
                    p11 = make_pair(*ivX1, *ivY1);
                    p12 = make_pair(*(ivX1+1), *(ivY1+1));
                    p21 = make_pair(*ivX2, *ivY2);
                    p22 = make_pair(*(ivX2+1), *(ivY2+1));
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1] && ix <= x12)
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                        }
                        b = true;  // f1 is smaller
                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }
                    }
                    vrX.push_back(x12);
                    vrY.push_back(y12);
                    vrP.push_back(*(ivP1+1));
                }
            }

            ivX1++;
            ivY1++;
            ivP1++;
        }
        else if(*(ivX1+1) > *(ivX2+1))
        {
//			cout << *(ivX1+1) << "\t" << *(ivX2+1) << endl;
            y1 = getY(*(ivX2+1));
            y2 = *(ivY2+1);

            if(b) // f1 is smaller
            {
//				cout << "B1 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 <= y2)
                {
                }
                else
                {
                    p11 = make_pair(x11, y11);
                    p12 = make_pair(x12, y12);
                    p21 = make_pair(x21, y21);
                    p22 = make_pair(x22, y22);
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1] && ix <= x12)
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                            b = false;
                        }

                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }
                    }

                    vrX.push_back(x22);
                    vrY.push_back(y22);
                    vrP.push_back(*(ivP2+1));
                }
            }
            else // f2 is smaller
            {
//				cout << "B2 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 < y2)
                {
                    p11 = make_pair(*ivX1, *ivY1);
                    p12 = make_pair(*(ivX1+1), *(ivY1+1));
                    p21 = make_pair(*ivX2, *ivY2);
                    p22 = make_pair(*(ivX2+1), *(ivY2+1));
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1] && x <= x12)
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                            b = true;
                        }
                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }
                    }
                }
                else
                {
                    vrX.push_back(*(ivX2+1));
                    vrY.push_back(y2);
                    vrP.push_back(*(ivP2+1));
                }
            }

            ivX2++;
            ivY2++;
            ivP2++;
        }
        else
        {
            y1 = *(ivY1+1);
            y2 = *(ivY2+1);
            if(b) // f1 is smaller
            {
//				cout << "C1 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 < y2)
                {
                    vrX.push_back(*(ivX1+1));
                    vrY.push_back(y1);
                    vrP.push_back(*(ivP2+1));
                }
                else if(y1 > y2)
                {
                    p11 = make_pair(*ivX1, *ivY1);
                    p12 = make_pair(*(ivX1+1), *(ivY1+1));
                    p21 = make_pair(*ivX2, *ivY2);
                    p22 = make_pair(*(ivX2+1), *(ivY2+1));
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1] && ix <= x12)
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                            b = false;
                        }
                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }
                    }
                    vrX.push_back(x22);
                    vrY.push_back(y2);
                    vrP.push_back(*(ivP2+1));
                }
                else
                {
                    vrX.push_back(*(ivX1+1));
                    vrY.push_back(y1);
                    vrP.push_back(*(ivP1+1));
                }
            }
            else // f1 is smaller
            {
//				cout << "C2 " << *(ivX1+1) << "," << y1 << "\t" << *(ivX2+1) << "," << y2 << endl;
//				cout << "(" << x11 << "," << y11 <<"),("  << x12 << "," << y12 << ")\t(" << x21 << "," << y21 << "),(" << x22 << "," << y22 << ")" << endl;
                if(y1 < y2)
                {
                    p11 = make_pair(*ivX1, *ivY1);
                    p12 = make_pair(*(ivX1+1), *(ivY1+1));
                    p21 = make_pair(*ivX2, *ivY2);
                    p22 = make_pair(*(ivX2+1), *(ivY2+1));
                    // return:
                    // 1: (p11, (x,y))
                    // 2: (p21, (x,y))
                    // 3: Same line
                    int it = hasIntersection(p11, p12, p21, p22, ix, iy);
                    if(it == 1 || it == 2)
                    {
                        if(ix > vrX[vrX.size()-1])
                        {
                            vrX.push_back(ix);
                            vrY.push_back(iy);
                            b = true;
                        }
                        if(it == 1){
                            vrP.push_back(*(ivP1));
                        }
                        else{
                            vrP.push_back(*(ivP2));
                        }
                    }
                    vrX.push_back(*(ivX1+1));
                    vrY.push_back(y1);
                    vrP.push_back(*(ivP1+1));
                }
                else if(y1 > y2)
                {
                    vrX.push_back(*(ivX2+1));
                    vrY.push_back(y2);
                    vrP.push_back(*(ivP2+1));
                }
                else
                {
                    vrX.push_back(*(ivX1+1));
                    vrY.push_back(y1);
                    vrP.push_back(*(ivP1+1));
                }
            }

            ivX1++;
            ivX2++;
            ivY1++;
            ivY2++;

            ivP1++;
            ivP2++;

            if(y1 == y2 && ivX1+1 != vX.end())
            {
                if(*(ivY1+1) <= *(ivY2+1))
                    b = true;
                else
                    b = false;
            }
        }

        int newy = vrY[vrY.size()-1];
        if(newy < m)
            m = newy;
//		if(vrX.size()>2)
//		cout << "new vrxy:" << vrX[vrX.size()-2] << "," << vrY[vrY.size()-2] << "\t" << vrX[vrX.size()-1] << "," << vrY[vrY.size()-1] << endl;
    }

    while(ivX1 + 1 !=vX.end())
    {
        vrX.push_back(*(ivX1+1));
        vrY.push_back(*(ivY1+1));
        ivX1++;
        ivY1++;

        vrP.push_back(*(ivP1+1));
        ivP1++;
    }

    while(ivX2 + 1 != f2.vX.end())
    {
        vrX.push_back(*(ivX2+1));
        vrY.push_back(*(ivY2+1));
        ivX2++;
        ivY2++;

        vrP.push_back(*(ivP2+1));
        ivP2++;
    }

    int yLast = min(*(vY.end()-1), *(f2.vY.end()-1));
    if(yLast < *(vrY.end()-1))
    {
        *(vrY.end()-1) = yLast;
    }

/*	vector<int>::iterator ivX, ivY;
	cout << "Min result in MinNew3" << endl;
	for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
		cout << "(" << *ivX << "," << *ivY << ")\t";
	cout << endl;
*/
    vector<int>::iterator ivX, ivY;
    ivX2 = f2.vX.begin();
    ivY2 = f2.vY.begin();

    // Variables for vP
    vector<int>::iterator ivP;
    ivP2 = f2.vP.begin();

//    for(ivX = vrX.begin() + 1, ivY = vrY.begin() + 1, ivP = vrP.begin() + 1; ivX != vrX.end(); ivX++, ivY++, ivP1++)
//    {
//        int yTmp = f2.getY(*ivX);
//        if(yTmp < *ivY)
//            *ivY = yTmp;
//        while(*ivX2 <= *ivX && ivX2 != f2.vX.end())
//        {
//            ivX2++;
//            ivY2++;
//
//            ivP2++;
//        }
////        if(ivX+1 != vX.end() && ivX2 != f2.vX.end())
//        if(ivX+1 != vrX.end() && ivX2 != f2.vX.end())
//        {
//            if(*ivX2 < *(ivX+1))
//            {
//                int yTmp2 = getY(*ivX2);
//                if(yTmp2 > *ivY2)
//                {
//                    vrX.insert(ivX, *ivX2);
//                    vrY.insert(ivY, *ivY2);
//                    ivX--;
//                    ivY--;
//
//                    vrP.insert(ivP, *ivP2);
//                    ivP--;
//                }
//            }
//        }
//    }

    for (ivX = vrX.begin() + 1, ivY = vrY.begin() + 1, ivP = vrP.begin() + 1; (ivX + 1) != vrX.end(); ivX++, ivY++, ivP++)
    {
        int yTmp = f2.getY(*ivX);
        if (yTmp < *ivY)
            *ivY = yTmp;
        while (ivX2 != f2.vX.end() && *ivX2 <= *ivX)
        {
            ivX2++;
            ivY2++;

            ivP2++;
        }
        //        if(ivX+1 != vX.end() && ivX2 != f2.vX.end())
        if (ivX + 1 != vrX.end() && ivX2 != f2.vX.end()) {
            //if (ivX!= vrX.begin()+1 && ivX2 != f2.vX.begin()){
            if (*ivX2 < *(ivX + 1))
            {
                int yTmp2 = getY(*ivX2);
                if (yTmp2 > *ivY2)
                {
                    vrX.insert(ivX, *ivX2);
                    vrY.insert(ivY, *ivY2);
                    ivX--;
                    ivY--;

                    vrP.insert(ivP, *ivP2);
                    ivP--;
                }
            }
        }
    }

    LPFunction fr;
    fr.setID1(f2.ID1);
    fr.setID2(f2.ID2);
    fr.setUBound(upperBound);
//    fr.setArrival(false);
//    fr.setValue(vrX, vrY);
    fr.setValue2(vrX, vrY, vrP);
    //fr.display();
    return fr;

//	setValue(vrX, vrY);
///	fr.setMin(m);
/*	for(ivX = vrX.begin(), ivY = vrY.begin(); ivX != vrX.end(); ivX++, ivY++)
		if(*ivY < 0)
		{
			display();
			f2.display();
			fr.display();
			exit(0);
		}
*/
//	cout << "IN Min" << endl;
//	cout << "test f1" << endl;
/*	if(!fr.dominate(*this))
	{
		cout << "Cannot Dominate f1!" << endl;
		cout << "f1:" << endl;
		display();
		cout << "lpfmin:" << endl;
		fr.display();
	}
//	cout << "test f2" << endl;
	if(!fr.dominate(f2))
	{
		cout << "Cannot Dominate f2!" << endl;
		cout << "f1:" << endl;
		display();
		cout << "f2:" << endl;
		f2.display();
		cout << "lpfmin:" << endl;
		fr.display();
	}
*/
}

