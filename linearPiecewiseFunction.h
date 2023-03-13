#ifndef JUSTINTIMEROUTING_LINEARPIECEWISEFUNCTION_H
#define JUSTINTIMEROUTING_LINEARPIECEWISEFUNCTION_H

#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <math.h>
#include <string>
#include <sstream>
#include <queue>
//#include <google/dense_hash_map>

#define INF 999999999

using namespace std;
//using namespace google;


struct spValue
{
    int x = 0, y = 0, p = 0;
};

class LPF
{
    std::vector<spValue> sp;

};

//speed profile
class LPFunction
{
public:
    vector<int> vX, vY, vP;
    int			ID1, ID2;
    int	        upperBound; //global maximum vx
    int			minY; 
    int			maxY;
//    bool		bArrival;
    //True: a min cost function, in-label
    //False: a cost function, out-label, need to refine the upper bound
//    bool		bValid;

    int computeY(int x1, int x2, int y1, int y2, int x) const;
    //test if (x2,y2) can be removed
    bool	redundant(int x1, int y1, int x2, int y2, int x3, int y3);

public:
    int setValue(vector<int>& vX, vector<int>& vY);
    int setValue2(vector<int>& vX, vector<int>& vY, vector<int>& vP);

    LPFunction(vector<int>& vX, vector<int>& vY, vector<int>& vP, int id1, int id2, int uBound, int maxY, int minY, bool bArrival = false, bool bValid = true)
    {
        ID1 = id1;
        ID2 = id2;
        upperBound = uBound;
        this->vX = vX;
        this->vY = vY;
        this->vP = vP;
        this->minY = minY;
        this->maxY = maxY;

        while(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] > upperBound)
        {
            this->vX.erase(this->vX.end()-1);
            this->vY.erase(this->vY.end()-1);
            this->vP.erase(this->vP.end()-1);
        }

        if(this->vX[this->vX.size()-1] + this->vY[this->vY.size()-1] < upperBound)
        {
            this->vX.push_back(upperBound - *(this->vY.end()-1));
            this->vY.push_back(*(this->vY.end()-1));
            this->vP.push_back(*(this->vP.end()-1));
        }

    }

    LPFunction()
    {
        ID1 = -1;
        ID2 = -1;
        upperBound = -1;
        minY = INF;
        maxY = -1;
    }

    void	import(int id1, int id2, int uBound, int minY, int maxY, vector<int>& vX, vector<int>& vY);
    int		getID1()	const;
    int		getID2()	const;
    int		getSize()	const;
    int		getUBound() const;
    int     getMaxX()   const;
    void	setID1(int id1);
    void	setID2(int id2);
    void	setUBound(int uBound);
    void    setvP(vector<int> VP);
    bool    isArrival() const;
    void	setArrival(bool b);
    bool	isValid() const;
    void	setValid(bool b);
    void	setLast(int y);
    vector<int>&   returnVX();
    vector<int>&   returnVY();
    pair<int,int> intervalMM(int t1, int t2);//return min, max between [t1,t2]

    pair<int, int>	getTimeDomain() const;
    int				getY(int x)		const;
    vector<int>		getVY(vector<int> inX);
    vector<int>		getVYHistogram(int n);
    void			setMin(int m);
    int				getMin();
    int				getMax();
    int				calMin();
    int				calMax();
    void			calMM();
    int				getMinInterval(int x1, int x2);//find minY in [x1,x2]

    vector<int>		getXF2(LPFunction& f2);
    //x+f(x)=f2x
    //return x
    int				getX(int x1, int y1, int x2, int y2, int f2x) const;
    int				getX(int f2x);
    int				getX2(int x1, int y1, int x2, int y2, int y);

    bool			equal(LPFunction f2);
    LPFunction		LPFCat(LPFunction f2);

    LPFunction		LPFMin(LPFunction f2);
    LPFunction		LPFMinNew3(LPFunction f2); // 2 lpfs have the same interval
    void swapLine(pair<int, int>& p11, pair<int, int>& p12, pair<int, int>& p21, pair<int, int>& p22);
    int hasIntersection(pair<int, int> p11, pair<int, int> p12, pair<int, int> p21, pair<int, int> p22, int& x, int& y);
    double direction(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk);
    bool onSegment(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk);
    bool Segment(std::pair<int, int> pi, std::pair<int, int> pj, std::pair<int, int> pk);

    bool	dominate(LPFunction f2);
    bool	dominateDisplay(LPFunction f2);
    bool	dominate5(LPFunction f2);
    bool	dominate2(LPFunction f2);

    void display();
    string toString();

    vector<pair<int, int> > findMM(int iNum);//find Min Max, break whole interval into iNum sub-intervals

};

#endif //JUSTINTIMEROUTING_LINEARPIECEWISEFUNCTION_H
